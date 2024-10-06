/****************************************************************************
This file is part of Project Apollo - NASSP

Apollo Generalized Optics Program

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

#include "ApolloGeneralizedOpticsProgram.h"
#include "rtcc.h"

AGOPOutputs::AGOPOutputs()
{
	IMUAttitude = _V(0, 0, 0);
	bIsNearHorizon = false;
	pitch = yaw = 0.0;
	REFSMMAT = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
	REFSMMAT_Vehicle = 0;
}

AGOP::AGOP(RTCC *r) :RTCCModule(r)
{

}

void AGOP::Calc(const AGOPInputs &in, AGOPOutputs &out)
{
	out.output_text.clear();
	out.errormessage.clear();

	ephemeris = in.ephem;

	switch (in.Option)
	{
	case 1:
		CislunarNavigation(in, out);
		break;
	case 2:
		ReferenceBodyComputation(in, out);
		break;
	case 3:
		StarCatalog(in, out);
		break;
	case 4:
		AntennaPointing(in, out);
		break;
	case 5:
		PassiveThermalControl(in, out);
		break;
	case 6:
		HorizonAngles(in, out);
		break;
	case 7:
		OpticalSupportTable(in, out);
		break;
	case 8:
		StarSightingTable(in, out);
		break;
	case 9:
		LunarSurfaceAlignmentDisplay(in, out);
		break;
	}
}

void AGOP::CislunarNavigation(const AGOPInputs &in, AGOPOutputs &out)
{
	EphemerisData sv;
	MATRIX3 SMNB;
	VECTOR3 U_S, R_L, R_ZC, R_CL, U_CL, Vec1, Vec2, Vec3;
	double GMT, TA, SA;
	int i;
	bool err;

	// Get star unit vector in BRCS
	U_S = GetStarUnitVector(in, in.StarIDs[0]);

	out.output_text.push_back("                  OST CISLUNAR NAVIGATION");
	out.output_text.push_back("   GET STAR ID HORZ OPTICS ANGLES INERTIAL ATTITUDE");
	out.output_text.push_back("HR:MIN:SEC DEC/OCT N-F   SFT     TRN     R      P      Y");
	//out.output_text.push_back("XXX:XX:XX XXX/XXX XXXX +XXX.XX +XX.XXX XXX.XX XXX.XX XXX.XX");

	GMT = ephemeris.Header.TL;
	i = 0;

	do
	{
		err = Interpolation(GMT, sv);
		if (err)
		{
			WriteError(out, 4);
			return;
		}

		//Convert position vector to desired SOI
		R_ZC = sv.R;

		if (in.Mode == 1 || in.Mode == 3)
		{
			if (sv.RBI != BODY_EARTH)
			{
				//Convert to Earth
				int err = pRTCC->ELVCNV(R_ZC, sv.GMT, 1, RTCC_COORDINATES_MCI, RTCC_COORDINATES_ECI, R_ZC);
				if (err)
				{
					WriteError(out, 1);
					return;
				}
			}
		}
		else
		{
			if (sv.RBI != BODY_MOON)
			{
				//Convert to Moon
				int err = pRTCC->ELVCNV(R_ZC, sv.GMT, 1, RTCC_COORDINATES_ECI, RTCC_COORDINATES_MCI, R_ZC);
				if (err)
				{
					WriteError(out, 1);
					return;
				}
			}
		}

		// Get horizon/landmark vector in BRCS
		if (in.Mode == 1 || in.Mode == 2)
		{
			// Horizon

			MATRIX3 M;
			VECTOR3 U_Z, u0, u1, u2, R_H, U_sH, t[2];
			double a_H, b_H, x_H, y_H, A, alpha, beta, AA[2];

			U_Z = _V(0, 0, 1);

			u2 = unit(crossp(U_S, R_ZC));
			u0 = unit(crossp(U_Z, u2));
			u1 = crossp(u2, u0);

			M = _M(u0.x, u0.y, u0.z, u1.x, u1.y, u1.z, u2.x, u2.y, u2.z);

			if (in.Mode == 1)
			{
				// Earth
				double SINL, r_F, h;

				SINL = dotp(u1, U_Z);

				r_F = OrbMech::R_Earth; //TBD
				h = 28000.0; //TBD
				a_H = r_F + h; //TBD
				b_H = r_F + h; //TBD
			}
			else
			{
				// Moon

				a_H = OrbMech::R_Moon;
				b_H = OrbMech::R_Moon;
			}

			R_H = mul(M, R_ZC);
			U_sH = mul(M, U_S);

			x_H = R_H.x;
			y_H = R_H.y;

			A = x_H * x_H / a_H / a_H + y_H * y_H / b_H / b_H;

			alpha = a_H / b_H * y_H*sqrt(A - 1.0);
			beta = b_H / a_H * x_H*sqrt(A - 1.0);

			t[0] = _V(x_H + alpha, y_H - beta, 0.0) / A;
			t[1] = _V(x_H - alpha, y_H + beta, 0.0) / A;

			AA[0] = dotp(U_sH, unit(t[0] - R_H));
			AA[1] = dotp(U_sH, unit(t[1] - R_H));

			VECTOR3 t_n, t_f;

			if (AA[1] > AA[0])
			{
				// 1 is near horizon
				t_n = t[1];
				t_f = t[0];
			}
			else
			{
				// 0 is near horizon
				t_n = t[0];
				t_f = t[1];
			}

			VECTOR3 R_L_v[2]; //0 = near, 1 = far horizon
			VECTOR3 R_EM, V_EM, R_ES;

			R_L_v[0] = tmul(M, t_n);
			R_L_v[1] = tmul(M, t_f);

			// Check which site has the higher sun elevation angle

			pRTCC->PLEFEM(1, sv.GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL);

			VECTOR3 N, rho, rho_apo;
			double sinang[2];

			for (unsigned i = 0; i < 2; i++)
			{
				//Unit horizon vector
				N = unit(R_L_v[i]);

				if (in.Mode == 1 || in.Mode == 3)
				{
					// Earth
					rho = R_ES - R_L_v[i];
				}
				else
				{
					// Moon
					rho = (R_ES - R_EM) - R_L_v[i];
				}

				//Unit vector from horizon vector to sun
				rho_apo = unit(rho);
				sinang[i] = dotp(rho_apo, N);
			}

			if (sinang[0] > sinang[1])
			{
				out.bIsNearHorizon = true;
			}
			else
			{
				out.bIsNearHorizon = false;
			}

			if (out.bIsNearHorizon)
			{
				//Use near horizon solution
				R_L = R_L_v[0];
			}
			else
			{
				//Use far horizon solution
				R_L = R_L_v[1];
			}
		}
		else
		{
			// Landmark

			// Convert to inertial
			if (in.Mode == 3)
			{
				bool err = GetInertialLandmarkVector(in.lmk_lat, in.lmk_lng, in.lmk_alt, sv.GMT, true, R_L);
				if (err)
				{
					WriteError(out, 1);
					return;
				}
			}
			else
			{
				bool err = GetInertialLandmarkVector(in.lmk_lat, in.lmk_lng, in.lmk_alt, sv.GMT, false, R_L);
				if (err)
				{
					WriteError(out, 1);
					return;
				}
			}
		}

		R_CL = R_L - R_ZC;
		U_CL = unit(R_CL);

		Vec3 = U_CL;
		Vec2 = unit(crossp(U_S, U_CL));
		Vec1 = unit(crossp(Vec2, U_CL));
		SMNB = mul(OrbMech::SBNBMatrix(), _M(Vec1.x, Vec1.y, Vec1.z, Vec2.x, Vec2.y, Vec2.z, Vec3.x, Vec3.y, Vec3.z));

		out.IMUAttitude = OrbMech::CALCGAR(in.CSM_REFSMMAT, SMNB);

		OrbMech::CALCSXA(SMNB, U_S, TA, SA);

		//Write line
		std::string line;
		char Buffer[128];

		OrbMech::format_time_HHMMSS(Buffer, pRTCC->GETfromGMT(sv.GMT));
		line.assign(Buffer);

		sprintf(Buffer, " %03d/%03o ", in.StarIDs[0], in.StarIDs[0]);
		line.append(Buffer);

		if (in.Mode == 1 || in.Mode == 2)
		{
			if (out.bIsNearHorizon)
			{
				line.append("NEAR ");
			}
			else
			{
				line.append(" FAR ");
			}
		}
		else
		{
			line.append("     ");
		}

		sprintf(Buffer, "%+07.2lf %+07.3lf %06.2lf %06.2lf %06.2lf", SA*DEG, TA*DEG, out.IMUAttitude.x*DEG, out.IMUAttitude.y*DEG, out.IMUAttitude.z*DEG);
		line.append(Buffer);

		out.output_text.push_back(line);
		i++;

		GMT = GMT + in.DeltaT;
		if (GMT > ephemeris.Header.TR) break;

	} while (i < 10);
}

void AGOP::ReferenceBodyComputation(const AGOPInputs &in, AGOPOutputs &out)
{
	std::string line;
	EphemerisData sv;
	VECTOR3 R_EM, V_EM, R_ES;
	double GMT;
	int i;
	bool err;
	char Buffer[128];

	sprintf(Buffer, "MODE %d   REFERENCE BODY COMPUTATION", in.Mode);
	out.output_text.push_back(Buffer);

	if (in.Mode != 1)
	{
		out.output_text.push_back("   GET         RA         DEC          UNIT VECTOR        ");
		out.output_text.push_back("HR:MIN:SEC HR:MIN:SEC HR:MIN:SEC                          ");
		//out.output_text.push_back("XXX:XX:XX  XXX:XX:XX  +XX:XX:XX +0.XXXXX +0.XXXXX +0.XXXXX");
	}

	GMT = ephemeris.Header.TL;
	i = 0;

	do
	{
		err = Interpolation(GMT, sv);
		if (err)
		{
			WriteError(out, 4);
			return;
		}

		err = pRTCC->PLEFEM(1, sv.GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL);

		if (err)
		{
			WriteError(out, 2);
			return;
		}

		OrbMech::format_time_HHMMSS(Buffer, pRTCC->GETfromGMT(sv.GMT));
		line.assign(Buffer);
		line += " ";

		if (in.Mode == 1)
		{
			//Calculate RA and declination of SC wrt the Earth, calculate RA and declination of Earth, Moon, Sun wrt the SC

			out.output_text.push_back("   GET         SPACECRAFT             EARTH    ");
			out.output_text.push_back("HR:MIN:SEC    RA        DEC       RA        DEC");
			//out.output_text.push_back("XXX:XX:XX XXX:XX:XX +XX:XX:XX XXX:XX:XX +XX:XX:XX");

			VECTOR3 R_EV; //Vector of vehicle wrt the Earth
			VECTOR3 u_EV; //Unit vector from Earth to vehicle
			VECTOR3 u_VE; //Unit vector from vehicle to Earth
			VECTOR3 u_VM; //Unit vector from vehicle to Moon
			VECTOR3 u_VS; //Unit vector from vehicle to Sun

			if (sv.RBI == BODY_EARTH)
			{
				R_EV = sv.R;
			}
			else
			{
				R_EV = sv.R + R_EM;
			}

			u_EV = unit(sv.R);
			u_VE = -u_EV;

			u_VM = unit(R_EM - sv.R);
			u_VS = unit(R_ES - sv.R);

			double decl, ra;

			OrbMech::latlong_from_r(u_EV, decl, ra);
			RightAscension_Display(Buffer, ra*DEG);
			line.append(Buffer);
			line += " ";
			Declination_Display(Buffer, decl*DEG);
			line.append(Buffer);
			line += " ";

			OrbMech::latlong_from_r(u_VE, decl, ra);
			RightAscension_Display(Buffer, ra*DEG);
			line.append(Buffer);
			line += " ";
			Declination_Display(Buffer, decl*DEG);
			line.append(Buffer);
			line += " ";
			out.output_text.push_back(line);
			out.output_text.push_back("");

			out.output_text.push_back("                  MOON                 SUN       ");
			out.output_text.push_back("              RA        DEC       RA        DEC  ");
			//out.output_text.push_back("          XXX:XX:XX +XX:XX:XX XXX:XX:XX +XX:XX:XX");

			line = "          ";
			OrbMech::latlong_from_r(u_VM, decl, ra);
			RightAscension_Display(Buffer, ra*DEG);
			line.append(Buffer);
			line += " ";
			Declination_Display(Buffer, decl*DEG);
			line.append(Buffer);
			line += " ";

			OrbMech::latlong_from_r(u_VS, decl, ra);
			RightAscension_Display(Buffer, ra*DEG);
			line.append(Buffer);
			line += " ";
			Declination_Display(Buffer, decl*DEG);
			line.append(Buffer);
			
			out.output_text.push_back(line);

			//Only for the first state vector
			return;
		}
		else
		{
			VECTOR3 u;
			double ra, decl;

			if (in.Mode == 2)
			{
				//Compute RA, declination, unit vector from spaceraft to center of Earth

				VECTOR3 R_EV;

				if (sv.RBI == BODY_EARTH)
				{
					R_EV = sv.R;
				}
				else
				{
					R_EV = sv.R + R_EM;
				}
				u = -unit(R_EV);
			}
			else if (in.Mode == 3)
			{
				//Compute RA, declination, unit vector from spaceraft to center of Moon

				VECTOR3 R_VM;

				if (sv.RBI == BODY_EARTH)
				{
					R_VM = R_EM - sv.R;
				}
				else
				{
					R_VM = -sv.R;
				}

				u = unit(R_VM);
			}
			else if (in.Mode == 4)
			{
				//Compute RA, declination, unit vector from spaceraft to center of Sun

				VECTOR3 R_VS;

				if (sv.RBI == BODY_EARTH)
				{
					R_VS = R_ES - sv.R;
				}
				else
				{
					R_VS = R_ES - (sv.R + R_EM);
				}

				u = unit(R_VS);
			}
			else if (in.Mode == 5)
			{
				//Compute RA, declination, unit vector from spaceraft to Earth landmark

				VECTOR3 R_L;

				bool err = GetInertialLandmarkVector(in.lmk_lat, in.lmk_lng, in.lmk_alt, sv.GMT, true, R_L);
				if (err)
				{
					WriteError(out, 1);
					return;
				}

				if (sv.RBI == BODY_EARTH)
				{
					u = unit(R_L - sv.R);
				}
				else
				{
					u = unit(R_L - (sv.R + R_EM));
				}
			}
			else
			{
				//Compute RA, declination, unit vector from spaceraft to Moon landmark

				VECTOR3 R_L;

				bool err = GetInertialLandmarkVector(in.lmk_lat, in.lmk_lng, in.lmk_alt, sv.GMT, false, R_L);
				if (err)
				{
					WriteError(out, 1);
					return;
				}

				if (sv.RBI == BODY_EARTH)
				{
					u = unit(R_L - (sv.R - R_EM));
				}
				else
				{
					u = unit(R_L - sv.R);
				}
			}

			OrbMech::latlong_from_r(u, decl, ra);
			RightAscension_Display(Buffer, ra*DEG);
			line.append(Buffer);
			line += "  ";
			Declination_Display(Buffer, decl*DEG);
			line.append(Buffer);
			line += "  ";

			for (unsigned i = 0; i < 3; i++)
			{
				sprintf(Buffer, "%+.5lf ", u.data[i]);
				line.append(Buffer);
			}

			out.output_text.push_back(line);
		}
		i++;

		GMT = GMT + in.DeltaT;
		if (GMT > ephemeris.Header.TR) break;

	} while (i < 10);
}

void AGOP::StarCatalog(const AGOPInputs &in, AGOPOutputs &out)
{
	std::string line;
	char Buffer[128];
	VECTOR3 u;
	double ra, decl;

	u = in.startable[in.StarIDs[0] - 1];

	out.output_text.push_back("                 STAR CATALOG");
	out.output_text.push_back("STAR ID     RA        DEC            UNIT VECTOR");
	out.output_text.push_back("DEC/OCT HR:MIN:SEC HR:MIN:SEC");
	//out.output_text.push_back("XXX/XXX XXX:XX:XX  +XX:XX:XX  +0.XXXXX +0.XXXXX +0.XXXXX");

	sprintf(Buffer, "%03d/%03o ", in.StarIDs[0], in.StarIDs[0]);
	line.assign(Buffer);

	OrbMech::latlong_from_r(u, decl, ra);
	RightAscension_Display(Buffer, ra*DEG);
	line.append(Buffer);
	line += "  ";
	Declination_Display(Buffer, decl*DEG);
	line.append(Buffer);
	line += "  ";

	for (unsigned i = 0; i < 3; i++)
	{
		sprintf(Buffer, "%+.5lf ", u.data[i]);
		line.append(Buffer);
	}

	out.output_text.push_back(line);
}

void AGOP::AntennaPointing(const AGOPInputs &in, AGOPOutputs &out)
{
	EphemerisData sv;
	MATRIX3 SMNB;
	VECTOR3 R_LMK, R, CSM_Att, LM_Att;
	double lat, lng, alt, CSM_PCH, CSM_YAW, LM_PCH, LM_YAW, SLANT_RANGE, AZM, ELV, GMT;
	int i;
	bool err;
	std::string line;
	char Buffer[128];

	out.output_text.push_back("    STEERABLE ANTENNA POINTING PROGRAM");
	sprintf(Buffer, "MODE %d ", in.Mode);
	line.assign(Buffer);
	if (in.Mode == 1 || in.Mode == 4)
	{
		line.append("ACTIVE VEH CSM ");
	}
	else
	{
		line.append("ACTIVE VEH LEM ");
	}

	if (in.AttIsCSM)
	{
		line.append("POINTING VEH CSM");
	}
	else
	{
		line.append("POINTING VEH LEM");
	}
	out.output_text.push_back(line);

	out.output_text.push_back("          ********CSM********  *********LM********");
	out.output_text.push_back("    GET   PCH YAW OGA IGA MGA  PCH YAW OGA IGA MGA");
	//out.output_text.push_back("XXX:XX:XX XXX XXX XXX XXX XXX  XXX XXX XXX XXX XXX");

	CSM_PCH = CSM_YAW = LM_PCH = LM_YAW = SLANT_RANGE = AZM = ELV = 0.0;
	CSM_Att = LM_Att = _V(0, 0, 0);

	//Find ground station
	if (in.GroundStationID == "")
	{
		lat = in.lmk_lat;
		lng = in.lmk_lng;
		alt = in.lmk_alt;
	}
	else
	{
		StationData stat;
		int j;

		bool found = false;
		for (j = 0; j < pRTCC->SystemParameters.MKRBKS; j++)
		{
			if (in.GroundStationID == pRTCC->GZSTCH[j].data.code)
			{
				found = true;
				break;
			}
		}
		if (found == false)
		{
			WriteError(out, 3);
			return;
		}
		stat = pRTCC->GZSTCH[j].data;

		lat = stat.lat_geoc;
		lng = stat.lng;
		alt = stat.H;
	}

	GMT = ephemeris.Header.TL;
	i = 0;

	do
	{
		err = Interpolation(GMT, sv);
		if (err)
		{
			WriteError(out, 4);
			return;
		}

		GetInertialLandmarkVector(lat, lng, alt, sv.GMT, true, R_LMK);

		//Get vector from spacecraft to ground station
		if (sv.RBI == BODY_EARTH)
		{
			R = R_LMK - sv.R;
		}
		else
		{
			VECTOR3 R_EM, V_EM;
			bool err = pRTCC->PLEFEM(4, sv.GMT / 3600.0, 0, &R_EM, &V_EM, NULL, NULL);

			if (err)
			{
				WriteError(out, 2);
				return;
			}

			R = R_LMK - (R_EM + sv.R);
		}

		SLANT_RANGE = length(R);

		if (in.Mode <= 3)
		{
			if (in.AttIsCSM)
			{
				CSM_Att = in.IMUAttitude[0];
			}
			else
			{
				LM_Att = in.IMUAttitude[0];
			}
		}

		if (in.Mode == 1)
		{
			// S-Band HGA (movable)

			if (!in.AttIsCSM)
			{
				// Convert attitude to CSM
				CSM_Att = LMIMUtoCMIMUAngles(in.CSM_REFSMMAT, in.LM_REFSMMAT, in.IMUAttitude[0], in.DockingAngle);
				SMNB = OrbMech::CALCSMSC(CSM_Att);
			}
			else
			{
				SMNB = OrbMech::CALCSMSC(in.IMUAttitude[0]);
			}

			CSMHGAngles(R, SMNB, in.CSM_REFSMMAT, CSM_PCH, CSM_YAW);

			out.pitch = CSM_PCH;
			out.yaw = CSM_YAW;
		}
		else if (in.Mode == 2)
		{
			// S-Band Steerable (movable)

			if (in.AttIsCSM)
			{
				// Convert attitude to LM
				LM_Att = CSMIMUtoLMIMUAngles(in.CSM_REFSMMAT, in.LM_REFSMMAT, in.IMUAttitude[0], in.DockingAngle);
				SMNB = OrbMech::CALCSMSC(LM_Att);
			}
			else
			{
				SMNB = OrbMech::CALCSMSC(in.IMUAttitude[0]);
			}

			LMSteerableAngles(R, SMNB, in.LM_REFSMMAT, LM_PCH, LM_YAW);

			out.pitch = LM_PCH;
			out.yaw = LM_YAW;
		}
		else if (in.Mode == 3)
		{
			// RR (movable)

			if (in.AttIsCSM)
			{
				// Convert attitude to LM
				LM_Att = CSMIMUtoLMIMUAngles(in.CSM_REFSMMAT, in.LM_REFSMMAT, in.IMUAttitude[0], in.DockingAngle);
				SMNB = OrbMech::CALCSMSC(LM_Att);
			}
			else
			{
				SMNB = OrbMech::CALCSMSC(in.IMUAttitude[0]);
			}

			RRAngles(R, SMNB, in.LM_REFSMMAT, LM_PCH, LM_YAW);
		}
		else if (in.Mode == 4)
		{
			// S-Band HGA (fixed)

			VECTOR3 SCAXIS;

			CSM_PCH = in.AntennaPitch;
			CSM_YAW = in.AntennaYaw;

			SCAXIS = GetBodyFixedHGAVector(CSM_PCH, CSM_YAW);
			SMNB = ThreeAxisPointing(SCAXIS, unit(R), sv.R, sv.V, in.HeadsUp ? 0.0 : PI);

			CSM_Att = OrbMech::CALCGAR(in.CSM_REFSMMAT, SMNB);

			if (in.AttIsCSM == false)
			{
				//Convert to LM
				LM_Att = CSMIMUtoLMIMUAngles(in.CSM_REFSMMAT, in.LM_REFSMMAT, CSM_Att, in.DockingAngle);
			}
		}
		else if (in.Mode == 5)
		{
			// S-Band Steerable (fixed)

			VECTOR3 SCAXIS;

			LM_PCH = in.AntennaPitch;
			LM_YAW = in.AntennaYaw;

			SCAXIS = GetBodyFixedSteerableAntennaVector(LM_PCH, LM_YAW);
			SMNB = ThreeAxisPointing(SCAXIS, unit(R), sv.R, sv.V, in.HeadsUp ? 0.0 : PI);

			LM_Att = OrbMech::CALCGAR(in.LM_REFSMMAT, SMNB);

			if (in.AttIsCSM)
			{
				//Convert to CSM
				CSM_Att = LMIMUtoCMIMUAngles(in.CSM_REFSMMAT, in.LM_REFSMMAT, LM_Att, in.DockingAngle);
			}
		}
		else
		{
			// RR (fixed)

			VECTOR3 SCAXIS;

			LM_PCH = in.AntennaPitch;
			LM_YAW = in.AntennaYaw;

			SCAXIS = GetBodyFixedRRVector(LM_YAW, LM_PCH);
			SMNB = ThreeAxisPointing(SCAXIS, unit(R), sv.R, sv.V, in.HeadsUp ? 0.0 : PI);

			LM_Att = OrbMech::CALCGAR(in.LM_REFSMMAT, SMNB);

			if (in.AttIsCSM)
			{
				//Convert to CSM
				CSM_Att = LMIMUtoCMIMUAngles(in.CSM_REFSMMAT, in.LM_REFSMMAT, LM_Att, in.DockingAngle);
			}
		}

		//Write line
		OrbMech::format_time_HHMMSS(Buffer, pRTCC->GETfromGMT(sv.GMT));
		line.assign(Buffer);
		line += " ";

		sprintf(Buffer, "%03.0lf %03.0lf %03.0lf %03.0lf %03.0lf  %03.0lf %03.0lf %03.0lf %03.0lf %03.0lf", CSM_PCH*DEG, CSM_YAW*DEG, CSM_Att.x*DEG, CSM_Att.y*DEG, CSM_Att.z*DEG,
			LM_PCH*DEG, LM_YAW*DEG, LM_Att.x*DEG, LM_Att.y*DEG, LM_Att.z*DEG);
		line.append(Buffer);
		out.output_text.push_back(line);

		i++;

		GMT = GMT + in.DeltaT;
		if (GMT > ephemeris.Header.TR) break;

	} while (i < 10);
}

void AGOP::PassiveThermalControl(const AGOPInputs &in, AGOPOutputs &out)
{
	EphemerisData sv;
	MATRIX3 M_NB;
	VECTOR3 R_EV, R_EM, V_EM, R_ES, X_NB, Y_NB, Z_NB, u_VE, u_VS;
	double GMT;
	std::string line;
	int i;
	bool err;
	char Buffer[128];

	out.output_text.push_back("     PASSIVE THERMAL CONTROL     ");
	out.output_text.push_back("   GET            ATTITUDE       ");
	out.output_text.push_back("HR:MIN:SEC  OGA     IGA     MGA  ");
	//out.output_text.push_back("XXX:XX:XX +XXX.XX +XXX.XX +XXX.XX");

	GMT = ephemeris.Header.TL;
	i = 0;

	do
	{
		err = Interpolation(GMT, sv);
		if (err)
		{
			WriteError(out, 4);
			return;
		}

		err = pRTCC->PLEFEM(1, sv.GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL);

		if (err)
		{
			WriteError(out, 2);
			return;
		}

		if (sv.RBI == BODY_EARTH)
		{
			R_EV = sv.R;
		}
		else
		{
			R_EV = R_EM + sv.R;
		}

		u_VE = -unit(R_EV);
		u_VS = unit(R_ES - R_EV);

		X_NB = unit(crossp(u_VE, u_VS));
		Y_NB = -crossp(X_NB, u_VE);
		Z_NB = crossp(X_NB, Y_NB);

		M_NB = _M(X_NB.x, X_NB.y, X_NB.z, Y_NB.x, Y_NB.y, Y_NB.z, Z_NB.x, Z_NB.y, Z_NB.z);

		out.IMUAttitude = OrbMech::CALCGAR(in.CSM_REFSMMAT, M_NB);
		out.REFSMMAT = M_NB;
		out.REFSMMAT_Vehicle = RTCC_MPT_CSM;

		//Write line
		OrbMech::format_time_HHMMSS(Buffer, pRTCC->GETfromGMT(sv.GMT));
		line.assign(Buffer);
		line += " ";
		sprintf(Buffer, "%+07.2lf %+07.2lf %+07.2lf", out.IMUAttitude.x*DEG, out.IMUAttitude.y*DEG, out.IMUAttitude.z*DEG);
		line.append(Buffer);
		out.output_text.push_back(line);

		i++;

		GMT = GMT + in.DeltaT;
		if (GMT > ephemeris.Header.TR) break;

	} while (i < 10);
}

void AGOP::HorizonAngles(const AGOPInputs &in, AGOPOutputs &out)
{
	EphemerisData sv;
	MATRIX3 M_NB;
	VECTOR3 LVLHAtt;
	double R, GMT;
	int i;
	bool err;
	std::string line;
	char Buffer[128];

	out.output_text.push_back("        HORIZON ALIGNMENT        ");
	out.output_text.push_back("   GET            ATTITUDE       ");
	out.output_text.push_back("HR:MIN:SEC  OGA     IGA     MGA  ");
	//out.output_text.push_back("XXX:XX:XX +XXX.XX +XXX.XX +XXX.XX");

	GMT = ephemeris.Header.TL;
	i = 0;

	do
	{
		err = Interpolation(GMT, sv);
		if (err)
		{
			WriteError(out, 4);
			return;
		}

		if (sv.RBI == BODY_EARTH)
		{
			R = OrbMech::R_Earth;
		}
		else
		{
			R = pRTCC->BZLAND.rad[0];
		}

		LVLHAtt.y = -acos(R / length(sv.R));

		if (in.HeadsUp)
		{
			LVLHAtt.x = 0.0;
			LVLHAtt.y -= pRTCC->SystemParameters.MCGHZA;
		}
		else
		{
			LVLHAtt.x = PI;
			LVLHAtt.y += pRTCC->SystemParameters.MCGHZA;
		}

		if (in.Mode == 1)
		{
			LVLHAtt.z = 0.0;
		}
		else
		{
			LVLHAtt.z = PI;
		}

		M_NB = LVLHAttitude(LVLHAtt, sv.R, sv.V);

		out.IMUAttitude = OrbMech::CALCGAR(in.CSM_REFSMMAT, M_NB);

		//Write line
		OrbMech::format_time_HHMMSS(Buffer, pRTCC->GETfromGMT(sv.GMT));
		line.assign(Buffer);
		line += " ";
		sprintf(Buffer, "%+07.2lf %+07.2lf %+07.2lf", out.IMUAttitude.x*DEG, out.IMUAttitude.y*DEG, out.IMUAttitude.z*DEG);
		line.append(Buffer);
		out.output_text.push_back(line);

		i++;

		GMT = GMT + in.DeltaT;
		if (GMT > ephemeris.Header.TR) break;

	} while (i < 10);
}

void AGOP::OpticalSupportTable(const AGOPInputs &in, AGOPOutputs &out)
{
	if (in.Mode == 1)
	{
		LMHorizonCheck(in, out);
	}
	else if (in.Mode == 2)
	{
		OSTAlignmentManeuverCheck(in, out);
	}
	else if (in.Mode == 3)
	{
		OSTComputeREFSMMAT(in, out);
	}
	else if (in.Mode == 4)
	{
		DockingAlignment(in, out);
	}
	else if (in.Mode == 5)
	{
		PointAOTWithCSM(in, out);
	}
	else if (in.Mode == 6)
	{
		REFSMMAT2REFSMMAT(in, out);
	}
	else if (in.Mode == 7)
	{
		//Crescent align?
	}
}

void AGOP::LMHorizonCheck(const AGOPInputs &in, AGOPOutputs &out)
{
	EphemerisData sv;
	MATRIX3 M_BRCS_SM, M_SM_NB, M_BRCS_NB, M_NB;
	VECTOR3 U_X, U_Y, U_Z, X_NB, Y_NB, Z_NB, R1, R2, R_horiz;
	double LPD;

	//Calculate burn direction (roll, pitch)
	M_BRCS_SM = in.LM_REFSMMAT;
	M_SM_NB = OrbMech::CALCSMSC(in.IMUAttitude[0]);
	M_BRCS_NB = mul(M_SM_NB, M_BRCS_SM);

	U_X = tmul(M_BRCS_NB, _V(1, 0, 0)); //Burn direction
	U_Y = tmul(M_BRCS_NB, _V(0, 1, 0));
	U_Z = tmul(M_BRCS_NB, _V(0, 0, 1)); //Forward window direction

	//Only for the first SV
	sv = SingleStateVector();

	//Find horizon
	R1 = VectorPointingToHorizon(sv, U_X, true);
	R2 = VectorPointingToHorizon(sv, U_X, false);

	//Which is closer to +Z axis?
	if (dotp(U_Z, unit(R1)) > dotp(U_Z, unit(R2)))
	{
		R_horiz = R1;
	}
	else
	{
		R_horiz = R2;
	}

	//X-axis aligned with burn vector
	X_NB = U_X;
	//Z-axis aligned with horizon
	Z_NB = unit(R_horiz);
	//Y-axis to complete coordinate system
	Y_NB = crossp(Z_NB, X_NB);

	M_NB = _M(X_NB.x, X_NB.y, X_NB.z, Y_NB.x, Y_NB.y, Y_NB.z, Z_NB.x, Z_NB.y, Z_NB.z);

	out.IMUAttitude = OrbMech::CALCGAR(in.LM_REFSMMAT, M_NB);

	//LPD
	R1 = VectorPointingToHorizon(sv, U_Y, true);
	R2 = VectorPointingToHorizon(sv, U_Y, false);

	//Which is closer to +Z axis?
	if (dotp(U_Z, unit(R1)) > dotp(U_Z, unit(R2)))
	{
		R_horiz = R1;
	}
	else
	{
		R_horiz = R2;
	}

	LPD = asin(dotp(-R_horiz, U_X));

	std::string line;
	char Buffer[128];

	sprintf(Buffer, "MODE %d  OPTICAL SIGHTING TABLE  VEH LM", in.Mode);
	out.output_text.push_back(Buffer);
	out.output_text.push_back("***BURN HORIZON CHECK***");

	line = "GETHOR ";
	OrbMech::format_time_HHMMSS(Buffer, pRTCC->GETfromGMT(sv.GMT));
	line.append(Buffer);

	sprintf(Buffer, " IMU %05.1lf LPD ", out.IMUAttitude.x*DEG);
	line.append(Buffer);
	if (LPD < 0.0 || LPD > 70.0*RAD)
	{
		line.append("N/A");
	}
	else
	{
		sprintf(Buffer, "%.1lf", LPD*DEG);
		line.append(Buffer);
	}
	out.output_text.push_back(line);
}

void AGOP::OSTAlignmentManeuverCheck(const AGOPInputs &in, AGOPOutputs &out)
{
	//Options here include
	//Sextant, LM COAS, AOT
	//Star search or input
	//Starting star for star search

	//Initial calculations
	RTCC::EMMENVInputTable intab;
	RTCC::EMMENVOutputTable outtab;
	MATRIX3 M_BRCS_NB;
	VECTOR3 u_BRCS, S_NB;
	double pitch, yaw, GMTAOS, GMTLOS;
	unsigned int star, num, search;
	bool inLimit, AOSFlag, LOSFlag;
	std::string line;
	char Buffer[128];

	//Format display
	//MODE 2  OPTICAL SIGHTING TABLE  VEH XXX
	//*******************BODY ATTITUDES*******************
	//     OGA XXX.XX
	//     IGA XXX.XX
	//     MGA XXX.XX
	//************ALIGNMENT AND MANEUVER CHECK************
	//          XXXXXXXXXX
	// STAR DEC OCT    XXX    XXX       AOS       LOS
	//  X/XXX   XXX   XXXXX  XXXXX  *XXX:XX:XX *XXX:XX:XX
	line = "MODE 2  OPTICAL SIGHTING TABLE  VEH ";


	if (in.AttIsCSM)
	{
		line += "CSM";
	}
	else
	{
		line += "LEM";
	}
	out.output_text.push_back(line);
	out.output_text.push_back("*******************BODY ATTITUDES*******************");

	line = "     OGA ";
	sprintf(Buffer, "%06.2lf", in.IMUAttitude[0].x*DEG);
	line.append(Buffer);
	out.output_text.push_back(line);

	line = "     IGA ";
	sprintf(Buffer, "%06.2lf", in.IMUAttitude[0].y*DEG);
	line.append(Buffer);
	out.output_text.push_back(line);

	line = "     MGA ";
	sprintf(Buffer, "%06.2lf", in.IMUAttitude[0].z*DEG);
	line.append(Buffer);
	out.output_text.push_back(line);

	out.output_text.push_back("************ALIGNMENT AND MANEUVER CHECK************");
	line = "          ";
	if (in.Instrument == 0)
	{
		line += "SXT";
	}
	else if (in.Instrument == 1)
	{
		if (in.LMCOASAxis)
		{
			line += "LM COAS +Z";
		}
		else
		{
			line += "LM COAS +X";
		}
	}
	else if (in.Instrument == 2)
	{
		line += "AOT";
	}
	else
	{
		line += "CSM COAS";
	}
	out.output_text.push_back(line);

	line = " STAR DEC OCT    ";
	if (in.Instrument == 0)
	{
		line += "SFT    TRN";
	}
	else if (in.Instrument == 1)
	{
		line += " AZ     EL";
	}
	else if (in.Instrument == 2)
	{
		line += " A1     A2";
	}
	else
	{
		line += "SPA    SXP";
	}

	line += "       AOS       LOS";
	out.output_text.push_back(line);

	//Calculate BRCS to NB matrix, NB of the vehicle with the instrument
	M_BRCS_NB = BRCStoNBMatrix(in, 0);

	//Select first star
	if (in.AdditionalOption == 0)
	{
		star = in.StartingStar;
	}
	else
	{
		//TBD: Input stars
		star = in.StarIDs[0];
	}

	num = 0; //Number of valid stars found
	search = 1; //Counter for input stars

	do
	{
		//Search for up to 10 stars

		//Get star vector in BRCS
		u_BRCS = GetStarUnitVector(in, star);

		//Convert to navigation base
		S_NB = mul(M_BRCS_NB, u_BRCS);

		inLimit = InstrumentLimitCheck(in, S_NB);

		//Check if the star is visible
		if (inLimit)
		{
			//Calculate AOS time
			intab.GMT = in.ephem.Header.TL;
			intab.option = 2;
			intab.terminator = false;
			intab.present = true;
			intab.u_vec = u_BRCS;
			pRTCC->EMMENV(ephemeris, mantimes, NULL, intab, outtab);

			//Found time or non-fatal error?
			if (outtab.err <= 1)
			{
				//Remember if this was an actual AOS or if line-of-sight was free at input time
				AOSFlag = !outtab.IsActualChange;
				GMTAOS = outtab.T_Change;

				//LOS time
				intab.GMT = outtab.T_Change + 1.0;
				intab.present = false;
				pRTCC->EMMENV(ephemeris, mantimes, NULL, intab, outtab);

				//Remember if this was an actual LOS
				if (outtab.err <= 1)
				{
					LOSFlag = false;
					GMTLOS = outtab.T_Change;
				}
				else
				{
					LOSFlag = true;
					GMTLOS = ephemeris.Header.TR;
				}
				
				//Calculate instrument angles
				InstrumentAngles(S_NB, in.Instrument, in.AOTDetent, in.LMCOASAxis, pitch, yaw);

				//Write line
				//  X/XXX   XXX   XXXXX  XXXXX  *XXX:XX:XX *XXX:XX:XX
				if (in.Instrument == 2)
				{
					sprintf(Buffer, "  %d", in.AOTDetent);
					line.assign(Buffer);
				}
				else
				{
					line = "   ";
				}
				line += "/";
				sprintf(Buffer, "%03d", star);
				line.append(Buffer);
				line.append("   ");
				if (star < 045)
				{
					//AGC star
					sprintf(Buffer, "%03o   ", star);
					line.append(Buffer);
				}
				else
				{
					line.append("      ");
				}
				if (in.Instrument == 0)
				{
					//Sextant
					sprintf(Buffer, "%06.2lf %06.3lf", yaw*DEG, pitch*DEG);
				}
				else if (in.Instrument == 1)
				{
					//LM COAS
					sprintf(Buffer, " %+05.1lf  %+05.1lf", yaw*DEG, pitch*DEG);
				}
				else if (in.Instrument == 2)
				{
					//AOT
					sprintf(Buffer, "%06.2lf %06.2lf", pitch*DEG, yaw*DEG);
				}
				else
				{
					//CSM COAS
					sprintf(Buffer, " %+05.1lf   %+04.1lf", pitch*DEG, yaw*DEG);
				}

				line.append(Buffer);
				if (AOSFlag)
				{
					line += " *";
				}
				else
				{
					line += "  ";
				}
				OrbMech::format_time_HHMMSS(Buffer, pRTCC->GETfromGMT(GMTAOS));
				line.append(Buffer);
				if (LOSFlag)
				{
					line += " *";
				}
				else
				{
					line += "  ";
				}
				OrbMech::format_time_HHMMSS(Buffer, pRTCC->GETfromGMT(GMTLOS));
				line.append(Buffer);
				out.output_text.push_back(line);

				num++;
			}
		}

		//Check next star
		if (in.AdditionalOption == 0)
		{
			star++;
			if (star > 400) break;
		}
		else
		{
			search++;
			if (search > 10) break;
			star = in.StarIDs[search - 1];
		}
	} while (num < 10);
}

void AGOP::OSTComputeREFSMMAT(const AGOPInputs &in, AGOPOutputs &out)
{
	MATRIX3 M_SM_NB_A, M_SM_NB_B;
	VECTOR3 U_CBA, U_CBB, U_CBA_apo, U_CBB_apo, U_NBA_apo, U_NBB_apo;
	bool conv;

	M_SM_NB_A = OrbMech::CALCSMSC(in.IMUAttitude[0]);
	M_SM_NB_B = OrbMech::CALCSMSC(in.IMUAttitude[1]);

	U_CBA = GetStarUnitVector(in, in.StarIDs[0]);
	U_CBB = GetStarUnitVector(in, in.StarIDs[1]);

	U_NBA_apo = GetNBUnitVectorFromInstrument(in, 0);
	U_NBB_apo = GetNBUnitVectorFromInstrument(in, 1);

	//Convert to other vehicle?
	conv = PointInstrumentOfOtherVehicle(in);

	if (conv)
	{
		MATRIX3 M_NBCSM_NBLM;
		M_NBCSM_NBLM = OrbMech::CSMBodyToLMBody(in.DockingAngle);

		if (in.AttIsCSM)
		{
			//Convert NB vectors from LM to CSM
			U_NBA_apo = tmul(M_NBCSM_NBLM, U_NBA_apo);
			U_NBB_apo = tmul(M_NBCSM_NBLM, U_NBB_apo);
		}
		else
		{
			//Convert NB vectors from CSM to LM
			U_NBA_apo = mul(M_NBCSM_NBLM, U_NBA_apo);
			U_NBB_apo = mul(M_NBCSM_NBLM, U_NBB_apo);
		}
	}

	U_CBA_apo = tmul(M_SM_NB_A, U_NBA_apo);
	U_CBB_apo = tmul(M_SM_NB_B, U_NBB_apo);

	//Error check for vectors being too close to each other
	const double eps = 0.01*RAD;
	double arc1, arc2, starang;

	arc1 = acos(dotp(U_CBA, U_CBB));
	arc2 = acos(dotp(U_CBA_apo, U_CBB_apo));

	if (arc1 < eps || arc2 < eps)
	{
		//Error
		WriteError(out, 7);
		return;
	}

	out.REFSMMAT = OrbMech::AXISGEN(U_CBA_apo, U_CBB_apo, U_CBA, U_CBB);
	out.REFSMMAT_Vehicle = in.AttIsCSM ? RTCC_MPT_CSM : RTCC_MPT_LM;

	starang = abs(arc1 - arc2);

	//Display
	std::string line;
	char Buffer[128];

	line = "MODE 3  OPTICAL SIGHTING TABLE  VEH ";

	if (in.AttIsCSM)
	{
		line += "CSM";
	}
	else
	{
		line += "LEM";
	}
	out.output_text.push_back(line);
	out.output_text.push_back("");
	sprintf(Buffer, "XIXE %+.8lf XIYE %+0.8lf XIZE %+0.8lf", out.REFSMMAT.m11, out.REFSMMAT.m12, out.REFSMMAT.m13);
	out.output_text.push_back(Buffer);
	sprintf(Buffer, "YIXE %+.8lf YIYE %+0.8lf YIZE %+0.8lf", out.REFSMMAT.m21, out.REFSMMAT.m22, out.REFSMMAT.m23);
	out.output_text.push_back(Buffer);
	sprintf(Buffer, "ZIXE %+.8lf ZIYE %+0.8lf ZIZE %+0.8lf", out.REFSMMAT.m31, out.REFSMMAT.m32, out.REFSMMAT.m33);
	out.output_text.push_back(Buffer);
	out.output_text.push_back("");
	sprintf(Buffer, "Star angle difference: %.3lf°", starang*DEG);
	out.output_text.push_back(Buffer);
}

void AGOP::DockingAlignment(const AGOPInputs &in, AGOPOutputs &out)
{
	//Option 0: LM REFSMMAT from CSM REFSMMAT, CSM attitude, docking angle and LM gimbal angles
	//Option 1: LM gimbal angles from CSM REFSMMAT, LM REFSMMAT, CSM gimbal angles and docking angle
	//Option 2: CSM gimbal angles from CSM REFSMMAT, LM REFSMMAT, LM gimbal angles and docking angle
	//Option 3: CSM REFSMMAT from CSM gimbal angles, LM REFSMMAT, LM gimbal angles and docking angle
	//Coordinate Systems:
	//Navigation Base (NB)
	//Stable Member (SM)
	//Basic Reference Coordinate System (BRCS)
	//REFSMMAT is BRCS to SM

	MATRIX3 M_NBCSM_NBLM;
	VECTOR3 GA_CSM, GA_LM;

	GA_CSM = in.IMUAttitude[0];

	if (in.AttIsFDAI)
	{
		GA_LM = pRTCC->EMMGFDAI(in.IMUAttitude[1], false);
	}
	else
	{
		GA_LM = in.IMUAttitude[1];
	}

	M_NBCSM_NBLM = OrbMech::CSMBodyToLMBody(in.DockingAngle);

	if (in.AdditionalOption == 0)
	{
		MATRIX3 M_SMCSM_NBCSM, M_SMLM_NBLM, M_BRCS_NBCSM, M_BRCS_NBLM, M_BRCS_SMLM;

		M_SMCSM_NBCSM = OrbMech::CALCSMSC(in.IMUAttitude[0]);
		M_SMLM_NBLM = OrbMech::CALCSMSC(GA_LM);
		M_BRCS_NBCSM = mul(M_SMCSM_NBCSM, in.CSM_REFSMMAT);
		M_BRCS_NBLM = mul(M_NBCSM_NBLM, M_BRCS_NBCSM);
		M_BRCS_SMLM = mul(OrbMech::tmat(M_SMLM_NBLM), M_BRCS_NBLM);

		out.REFSMMAT = M_BRCS_SMLM;
		out.REFSMMAT_Vehicle = RTCC_MPT_LM;
	}
	else if (in.AdditionalOption == 1)
	{
		GA_LM = CSMIMUtoLMIMUAngles(in.CSM_REFSMMAT, in.LM_REFSMMAT, in.IMUAttitude[0], in.DockingAngle);
	}
	else if (in.AdditionalOption == 2)
	{
		GA_CSM = LMIMUtoCMIMUAngles(in.CSM_REFSMMAT, in.LM_REFSMMAT, GA_LM, in.DockingAngle);
	}
	else
	{
		MATRIX3 M_SMCSM_NBCSM, M_SMLM_NBLM, M_BRCS_NBLM, M_BRCS_NBCSM, M_BRCS_SMCSM;

		M_SMCSM_NBCSM = OrbMech::CALCSMSC(in.IMUAttitude[0]);
		M_SMLM_NBLM = OrbMech::CALCSMSC(GA_LM);
		M_BRCS_NBLM = mul(M_SMLM_NBLM, in.LM_REFSMMAT);
		M_BRCS_NBCSM = mul(OrbMech::tmat(M_NBCSM_NBLM), M_BRCS_NBLM);
		M_BRCS_SMCSM = mul(OrbMech::tmat(M_SMCSM_NBCSM), M_BRCS_NBCSM);

		out.REFSMMAT = M_BRCS_SMCSM;
		out.REFSMMAT_Vehicle = RTCC_MPT_CSM;
	}

	char Buffer[256];
	std::string line;

	//                 DOCKING ALIGNMENT PROCESSOR
	//                   XXX XXXXXXXX IS COMPUTED
	//              *******                     *******
	//              * CSM *                     * LEM *
	//              *******                     *******
	//         IMU GIMBAL ANGLES           IMU GIMBAL ANGLES
	//REFSMMAT OGA    IGA    MGA REFSMMAT  OGA    IGA    MGA
	//XXXXXX XXX.XX XXX.XX XXX.XX XXXXXX XXX.XX XXX.XX XXX.XX
	//
	//              CALCULATED REFSMMAT


	out.output_text.push_back("                 DOCKING ALIGNMENT PROCESSOR");

	line = "                   ";
	if (in.AdditionalOption == 0)
	{
		line += "LM REFSMMAT";
	}
	else if (in.AdditionalOption == 1)
	{
		line += "LM ATTITUDE";
	}
	else if (in.AdditionalOption == 2)
	{
		line += "CSM ATTITUDE";
	}
	else
	{
		line += "CSM REFSMMAT";
	}

	line += " IS COMPUTED";
	out.output_text.push_back(line);

	out.output_text.push_back("              *******                     *******");
	out.output_text.push_back("              * CSM *                     * LEM *");
	out.output_text.push_back("              *******                     *******");
	out.output_text.push_back("         IMU GIMBAL ANGLES           IMU GIMBAL ANGLES");
	out.output_text.push_back("REFSMMAT OGA    IGA    MGA REFSMMAT  OGA    IGA    MGA");

	line = "XXXXXX ";
	sprintf(Buffer, "%06.2lf %06.2lf %06.2lf ", GA_CSM.x*DEG, GA_CSM.y*DEG, GA_CSM.z*DEG);
	line.append(Buffer);
	line += "XXXXXX ";
	sprintf(Buffer, "%06.2lf %06.2lf %06.2lf ", GA_LM.x*DEG, GA_LM.y*DEG, GA_LM.z*DEG);
	line.append(Buffer);
	out.output_text.push_back(line);

	if (in.AdditionalOption == 0 || in.AdditionalOption == 3)
	{
		//Show calculated REFSMMAT
		out.output_text.push_back("              CALCULATED REFSMMAT");
		sprintf(Buffer, "%010.7lf %010.7lf %010.7lf", out.REFSMMAT.m11, out.REFSMMAT.m12, out.REFSMMAT.m13);
		out.output_text.push_back(Buffer);
		sprintf(Buffer, "%010.7lf %010.7lf %010.7lf", out.REFSMMAT.m21, out.REFSMMAT.m22, out.REFSMMAT.m23);
		out.output_text.push_back(Buffer);
		sprintf(Buffer, "%010.7lf %010.7lf %010.7lf", out.REFSMMAT.m31, out.REFSMMAT.m32, out.REFSMMAT.m33);
		out.output_text.push_back(Buffer);
	}
}

void AGOP::PointAOTWithCSM(const AGOPInputs &in, AGOPOutputs &out)
{
	MATRIX3 M_NBCSM_NBLM;
	EphemerisData sv;
	MATRIX3 SMNB;
	VECTOR3 SCAXIS, u_LOS, u_ANB, CSM_Att;
	double AZ, EL, OMICRON;

	//Get state vector
	sv = SingleStateVector();

	//LM NB vector pointing at the star
	GetAOTNBAngle(in.AOTDetent, AZ, EL);
	SCAXIS = OrbMech::AOTNavigationBase(AZ, EL);

	//BRCS vector pointing at the star
	u_LOS = GetStarUnitVector(in, in.StarIDs[0]);

	//CSM NB vector pointing at the star
	M_NBCSM_NBLM = OrbMech::CSMBodyToLMBody(in.DockingAngle);
	u_ANB = tmul(M_NBCSM_NBLM, SCAXIS);

	//CSM attitude
	for (int i = 0; i < 2; i++)
	{
		OMICRON = 0.0;
		SMNB = ThreeAxisPointing(u_ANB, u_LOS, sv.R, sv.V, OMICRON);
		CSM_Att = OrbMech::CALCGAR(in.CSM_REFSMMAT, SMNB);

		//Gimbal lock check
		if (cos(CSM_Att.z) > 0.2) break;
		OMICRON += 90.0*RAD;
	}

	char Buffer[256];
	std::string line;

	out.output_text.push_back("POINT AOT WITH CSM");
	out.output_text.push_back("");
	line = "CSM Gimbal Angles: ";

	sprintf(Buffer, "%06.2lf %06.2lf %06.2lf", CSM_Att.x*DEG, CSM_Att.y*DEG, CSM_Att.z*DEG);
	line.append(Buffer);
	out.output_text.push_back(line);
}

void AGOP::REFSMMAT2REFSMMAT(const AGOPInputs &in, AGOPOutputs &out)
{
	//CSM REFSMMAT is current REFSMMAT, LM REFSMMAT is preferred REFSMMAT, IMUAttitude[0] is attitude with preferred REFSMMAT. Calculate attitude with current REFSMMAT.

	MATRIX3 M_BRCS_SM, M_SM_NB, M_BRCS_NB;
	VECTOR3 Att, FDAI_Att;

	M_BRCS_SM = in.LM_REFSMMAT;
	M_SM_NB = OrbMech::CALCSMSC(in.IMUAttitude[0]);
	M_BRCS_NB = mul(M_SM_NB, M_BRCS_SM);

	Att = OrbMech::CALCGAR(in.CSM_REFSMMAT, M_BRCS_NB);

	if (in.AttIsCSM)
	{
		FDAI_Att = Att;
	}
	else
	{
		FDAI_Att = pRTCC->EMMGFDAI(Att, true);
	}

	char Buffer[256];
	std::string line;

	out.output_text.push_back("REFSMMAT TO REFSMMAT");
	out.output_text.push_back("");

	line = "Gimbal Angles: ";
	sprintf(Buffer, "%06.2lf %06.2lf %06.2lf", Att.x*DEG, Att.y*DEG, Att.z*DEG);
	line.append(Buffer);
	out.output_text.push_back(line);

	line = "FDAI Angles: ";
	sprintf(Buffer, "%06.2lf %06.2lf %06.2lf", FDAI_Att.x*DEG, FDAI_Att.y*DEG, FDAI_Att.z*DEG);
	line.append(Buffer);
	out.output_text.push_back(line);
}

//Option 8
void AGOP::StarSightingTable(const AGOPInputs &in, AGOPOutputs &out)
{
	VECTOR3 GA, u_star;
	double TGT_RT_ASC, TGT_DEC, LOS_RT_ASC, LOS_DEC, OPT_ANG1, OPT_ANG2, CA, GETCA;

	OPT_ANG1 = OPT_ANG2 = 0.0;
	CA = 0.0;
	GETCA = 0.0;

	if (in.Mode == 1 || in.Mode == 3)
	{
		// Landmark
		EphemerisDataTable2 ephemeris_true;
		std::vector<StationContact> acquisitions;
		StationContact acquisition;
		StationData Station;
		EphemerisData sv;
		double GMT, GMTCA;
		int body, csi_out, err;
		bool err2;

		// Assume the landmark is in the same SOI as the ephemeris, i.e. initial state vector
		body = in.ephem.Header.CSI == 0 ? BODY_EARTH : BODY_MOON;
		csi_out = in.ephem.Header.CSI + 1;

		// Create station characteristics table
		pRTCC->EMGGPCHR(in.lmk_lat, in.lmk_lng, in.lmk_alt, body, 0.0, &Station);
		// Convert ephemeris to ECT or MCT
		ephemeris_true = ephemeris;
		pRTCC->ELVCNV(ephemeris.table, in.ephem.Header.CSI, csi_out, ephemeris_true.table);
		ephemeris_true.Header.CSI = csi_out;
		// Calculate acquisitions
		err = pRTCC->EMXING(ephemeris_true, mantimes, Station, body, acquisitions, NULL);

		if (err || acquisitions.size() == 0)
		{
			// Didn't find any acquisitions
			WriteError(out, 5);
			return;
		}

		//Only use first acquisition
		acquisition = acquisitions[0];

		if (in.ElevationAngle > acquisition.MAXELEV)
		{
			//Didn't rise high enough
			WriteError(out, 5);
			return;
		}

		//Search elevation angle time from GMT of AOS (0° elev)
		err = FindLandmarkAOS(out, Station, acquisition.GMTAOS, in.ElevationAngle, GMT);

		if (err)
		{
			// Didn't find any acquisitions
			WriteError(out, 5);
			return;
		}

		GMTCA = acquisition.GMTEMAX;
		GETCA = pRTCC->GETfromGMT(GMTCA);

		err2 = Interpolation(GMTCA, sv);

		if (in.Mode == 1)
		{
			//Fixed instrument, calculate IMU attitude (heads-up)
		}
		else
		{
			//Fixed attitude, calculate instrument
		}
	}
	else if (in.Mode == 2 || in.Mode == 4)
	{
		// Star
		RTCC::EMMENVInputTable intab;
		RTCC::EMMENVOutputTable outtab;

		u_star = GetStarUnitVector(in, in.StarIDs[0]);

		// Calculate AOS
		intab.GMT = ephemeris.Header.TL;
		intab.option = 2;
		intab.terminator = false;
		intab.present = true;

		pRTCC->EMMENV(ephemeris, mantimes, NULL, intab, outtab);

		if (outtab.err > 1)
		{
			// No AOS in timespan
			WriteError(out, 6);
			return;
		}
	}
	else
	{
		// Imaginary star
		MATRIX3 M_BRCS_SM, M_SM_NB;
		VECTOR3 u_NB, u_SM;

		u_NB = GetNBUnitVectorFromInstrument(in, 0);

		if (in.AttIsCSM)
		{
			M_BRCS_SM = in.CSM_REFSMMAT;
		}
		else
		{
			M_BRCS_SM = in.LM_REFSMMAT;
		}
		GetAttitudeMatrix(in, 0, GA, M_SM_NB);

		u_SM = tmul(M_SM_NB, u_NB);
		u_star = tmul(M_BRCS_SM, u_SM);
	}

	if (in.Mode == 2 || in.Mode == 4)
	{
		OrbMech::latlong_from_r(u_star, TGT_DEC, TGT_RT_ASC);
	}
	else
	{
		TGT_DEC = TGT_RT_ASC = 0.0;
	}

	if (in.Mode == 2 || in.Mode == 4 || in.Mode == 5)
	{
		OrbMech::latlong_from_r(u_star, LOS_DEC, LOS_RT_ASC);
	}
	else
	{
		LOS_DEC = LOS_RT_ASC = 0.0;
	}

	std::string line;
	char Buffer[128];

	//                   STAR SIGHTING TABLE
	//
	//               VEHICLE XXX MODE X
	//  TGTID XXX
	//                       OG   XXX.XX
	// TGT DEC   +XX:XX:XX   IG   XXX.XX     LOS DEC    +XX:XX:XX
	//TGT RT ASC XXX:XX:XX   MG   XXX.XX     LOS RT ASC XXX:XX:XX
	//
	//
	//  GND PT DATA          OPTICS XXXX/XX  GETT XXX:XX:XX
	//                       XXX XXX.XXX     REV  XXX
	// LAT +XX.XXX           XXX XXX.XXX     LON  XXX.XXX
	// LONG XXX.XXX                          LON GET XXX:XX:XX
	// ALT  XXXXX 
	// ELV  XX                               CA   XXX.XX
	//                                       GETCA XXX:XX:XX
	//
	//
	//                    REFSMMAT

	out.output_text.push_back("                   STAR SIGHTING TABLE");
	out.output_text.push_back("");
	line = "               VEHICLE ";
	if (in.AttIsCSM)
	{
		line += "CSM";
	}
	else
	{
		line += "LEM";
	}
	line += " MODE ";
	sprintf(Buffer, "%d", in.Mode);
	line.append(Buffer);
	out.output_text.push_back(line);

	line = "  TGTID ";
	if (in.Mode == 1 || in.Mode == 3)
	{
		line += "LMK";
	}
	else if (in.Mode == 5)
	{
		line += "STAR";
	}
	else
	{
		sprintf(Buffer, "%03d", in.StarIDs[0]);
		line.append(Buffer);
	}
	out.output_text.push_back(line);

	line = "                       OG   ";
	sprintf(Buffer, "%06.2lf", GA.x*DEG);
	line.append(Buffer);
	out.output_text.push_back(line);

	line = " TGT DEC   ";
	if (in.Mode == 2 || in.Mode == 4)
	{
		Declination_Display(Buffer, TGT_DEC*DEG);
		line.append(Buffer);
	}
	else
	{
		line += "         ";
	}
	line += "   IG   ";
	sprintf(Buffer, "%06.2lf", GA.y*DEG);
	line.append(Buffer);
	line += "     LOS DEC    ";
	if (in.Mode == 2 || in.Mode == 4 || in.Mode == 5)
	{
		Declination_Display(Buffer, LOS_DEC*DEG);
		line.append(Buffer);
	}
	out.output_text.push_back(line);

	line = "TGT RT ASC ";
	if (in.Mode == 2 || in.Mode == 4)
	{
		RightAscension_Display(Buffer, TGT_RT_ASC*DEG);
		line.append(Buffer);
	}
	else
	{
		line += "         ";
	}
	line += "   MG   ";
	sprintf(Buffer, "%06.2lf", GA.z*DEG);
	line.append(Buffer);
	line += "     LOS RT ASC ";
	if (in.Mode == 2 || in.Mode == 4 || in.Mode == 5)
	{
		RightAscension_Display(Buffer, LOS_RT_ASC*DEG);
		line.append(Buffer);
	}
	out.output_text.push_back(line);
	out.output_text.push_back("");
	out.output_text.push_back("");
	out.output_text.push_back("");

	line = "  GND PT DATA          OPTICS ";
	if (in.Instrument == 0)
	{
		line += "SXT    ";
	}
	else if (in.Instrument == 1)
	{
		if (in.AttIsCSM)
		{
			line += "COAS   ";
		}
		else
		{
			if (in.LMCOASAxis)
			{
				line += "COAS +Z";
			}
			else
			{
				line += "COAS +X";
			}
		}
	}
	else
	{
		line += "AOT/";
		sprintf(Buffer, "%d", in.AOTDetent);
		line.append(Buffer);
	}
	line += "  GETT ";

	OrbMech::format_time_HHMMSS(Buffer, pRTCC->GETfromGMT(ephemeris.Header.TL));
	line.append(Buffer);
	out.output_text.push_back(line);

	line = "                       ";
	if (in.Instrument == 0)
	{
		line += "SFT";
	}
	else if (in.Instrument == 1)
	{
		line += "AZ ";
	}
	else if (in.Instrument == 2)
	{
		line += "A1 ";
	}
	else
	{
		line += "SPA";
	}

	line += " ";


}

//Option 9
void AGOP::LunarSurfaceAlignmentDisplay(const AGOPInputs &in, AGOPOutputs &out)
{
	//Calculate matrix from navigation base (vessel) to MCT (Moon fixed)

	MATRIX3 M_NB_MCT;

	if (in.Mode == 1)
	{
		//2 stars
		MATRIX3 M_MCI_MCT_1, M_MCI_MCT_2;
		VECTOR3 U_NBA, U_NBB, s_CBA, s_CBB, U_CBA, U_CBB;

		//2 vectors in NB coordinates
		if (in.Instrument == 1)
		{
			//COAS
			U_NBA = GetLMCOASVector(in.COASElevationAngle[0], in.COASPositionAngle[0], in.LMCOASAxis);
			U_NBB = GetLMCOASVector(in.COASElevationAngle[1], in.COASPositionAngle[1], in.LMCOASAxis);
		}
		else
		{
			//AOT
			double AZ, EL;

			GetAOTNBAngle(in.AOTDetent, AZ, EL);

			U_NBA = GetAOTNBVector(EL, AZ, in.AOTReticleAngle[0], in.AOTSpiraleAngle[0], in.AOTLineID[0]);
			U_NBB = GetAOTNBVector(EL, AZ, in.AOTReticleAngle[1], in.AOTSpiraleAngle[1], in.AOTLineID[1]);
		}

		//2 vectors in REF coordinates
		s_CBA = GetStarUnitVector(in, in.StarIDs[0]);
		s_CBB = GetStarUnitVector(in, in.StarIDs[1]);

		//Conversion from MCI to MCT
		pRTCC->ELVCNV(in.TimeOfSighting[0], RTCC_COORDINATES_MCI, RTCC_COORDINATES_MCT, M_MCI_MCT_1);
		pRTCC->ELVCNV(in.TimeOfSighting[1], RTCC_COORDINATES_MCI, RTCC_COORDINATES_MCT, M_MCI_MCT_2);

		//Star vectors to MCT
		U_CBA = mul(M_MCI_MCT_1, s_CBA);
		U_CBB = mul(M_MCI_MCT_2, s_CBB);

		M_NB_MCT = OrbMech::AXISGEN(U_CBA, U_CBB, U_NBA, U_NBB);
	}
	else if (in.Mode == 2)
	{
		//1 star and gravity

		MATRIX3 SMNB, M_MCI_MCT;
		VECTOR3 U_NBA, s_MCI, U_CBA, U_CBB, GA, G_NB, R_LS_MCT;
		double T_AOT;

		//Time at which input AOT angles are valid
		T_AOT = pRTCC->GMTfromGET(in.TimeOfSighting[0]);

		//Star vector in NB coordinates
		if (in.Instrument == 1)
		{
			//COAS
			U_NBA = GetLMCOASVector(in.COASElevationAngle[0], in.COASPositionAngle[0], in.LMCOASAxis);
		}
		else
		{
			//AOT
			double AZ, EL;

			GetAOTNBAngle(in.AOTDetent, AZ, EL);
			U_NBA = GetAOTNBVector(EL, AZ, in.AOTReticleAngle[0], in.AOTSpiraleAngle[0], in.AOTLineID[0]);
		}

		//Star vector in inertial (MCI) coordinates
		s_MCI = GetStarUnitVector(in, in.StarIDs[1]);

		if (in.AttIsFDAI)
		{
			GA = pRTCC->EMMGFDAI(in.IMUAttitude[0], false);
		}
		else
		{
			GA = in.IMUAttitude[0];
		}

		//Stable member to navigation base
		SMNB = OrbMech::CALCSMSC(GA);

		//Assuming gravity vector in SM coordinates is unit x vector, calculate gravity vector in navigation base coordinates
		G_NB = mul(SMNB, _V(1, 0, 0));

		//Get selenographic landing site vector
		R_LS_MCT = OrbMech::r_from_latlong(in.LSLat, in.LSLng);

		//Get matrix converting from MCI to MCT at time T_AOT
		pRTCC->ELVCNV(T_AOT, RTCC_COORDINATES_MCI, RTCC_COORDINATES_MCT, M_MCI_MCT);

		//Star vector in MCT coordinates
		U_CBA = mul(M_MCI_MCT, s_MCI);
		//unit landing site vector in MCT
		U_CBB = unit(R_LS_MCT);

		M_NB_MCT = OrbMech::AXISGEN(U_CBA, U_CBB, U_NBA, G_NB);
	}
	else if (in.Mode == 3)
	{
		//LVLH
		MATRIX3 M_LVLH_NB, M_MCT_LVLH;

		M_LVLH_NB = mul(OrbMech::_MRz(in.IMUAttitude[0].z), mul(OrbMech::_MRy(in.IMUAttitude[0].y), OrbMech::_MRx(in.IMUAttitude[0].x)));
		M_MCT_LVLH = _M(cos(in.LSLat)*cos(in.LSLng), cos(in.LSLat)*sin(in.LSLng), sin(in.LSLat), -sin(in.LSLng), cos(in.LSLng), 0.0, -sin(in.LSLat)*cos(in.LSLng), -sin(in.LSLat)*sin(in.LSLng), cos(in.LSLat));

		M_NB_MCT = OrbMech::tmat(mul(M_MCT_LVLH, M_LVLH_NB));
	}
	else if (in.Mode == 4)
	{
		//Input gimbal angles

		MATRIX3 SMNB, M_MCI_MCT, M_NB_MCI;
		VECTOR3 GA;

		if (in.AttIsFDAI)
		{
			GA = pRTCC->EMMGFDAI(in.IMUAttitude[0], false);
		}
		else
		{
			GA = in.IMUAttitude[0];
		}

		//Stable member to navigation base
		SMNB = OrbMech::CALCSMSC(GA);

		//Get matrix converting from MCI to MCT at time
		pRTCC->ELVCNV(pRTCC->GMTfromGET(in.TimeOfSighting[0]), RTCC_COORDINATES_MCI, RTCC_COORDINATES_MCT, M_MCI_MCT);

		//NB to MCI
		M_NB_MCI = OrbMech::tmat(mul(SMNB, in.LM_REFSMMAT));
		//NB to MCT
		M_NB_MCT = mul(M_MCI_MCT, M_NB_MCI);
	}

	//Format output
	out.output_text.push_back("        LUNAR SURFACE ALIGN                         ");
	out.output_text.push_back("CSM STA ID XXXXXXX  GETLO XXX:XX:XX                 ");
	out.output_text.push_back("   GMTV  XXX:XX:XX    PLM  +XX.XXXX GETR XXX:XX:XX  ");
	out.output_text.push_back("   GETV  XXX:XX:XX    LLM +XXX.XXXX L AZ XXX.X      ");
	out.output_text.push_back("MODE XXXXXXXXXXXX                                   ");
	out.output_text.push_back("PD XXX.X PA XXX.X PB XXX.X YH XXX.X  SINDL +X.XXXXXX");
	out.output_text.push_back("YD XXX.X YA XXX.X YB XXX.X PH XXX.X   047  +XXXXX   ");
	out.output_text.push_back("RD XXX.X RA XXX.X RB XXX.X RH XXX.X  SINDL +X.XXXXXX");
	out.output_text.push_back("                                      053  +XXXXX   ");
}

void AGOP::WriteError(AGOPOutputs &out, int err)
{
	switch (err)
	{
	case 1:
		out.errormessage = "UNABLE TO CONVERT VECTORS";
		break;
	case 2:
		out.errormessage = "EPHEMERIDES NOT AVAILABLE";
		break;
	case 3:
		out.errormessage = "GROUND STATION NOT FOUND";
		break;
	case 4:
		out.errormessage = "INTERPOLATION FAILURE";
		break;
	case 5:
		out.errormessage = "LANDMARK NOT IN SIGHT";
		break;
	case 6:
		out.errormessage = "NO AOS IN TIMESPAN";
		break;
	case 7:
		out.errormessage = "STARS TOO CLOSE TO EACH OTHER";
		break;
	}
}

void AGOP::RightAscension_Display(char *Buff, double angle)
{
	double angle2 = abs(round(angle*3600.0));
	sprintf_s(Buff, 32, "%03.0f:%02.0f:%02.0f", floor(angle2 / 3600.0), floor(fmod(angle2, 3600.0) / 60.0), fmod(angle2, 60.0));
}

void AGOP::Declination_Display(char *Buff, double angle)
{
	double angle2 = abs(round(angle*3600.0));
	if (angle >= 0)
	{
		sprintf_s(Buff, 32, "+%02.0f:%02.0f:%02.0f", floor(angle2 / 3600.0), floor(fmod(angle2, 3600.0) / 60.0), fmod(angle2, 60.0));
	}
	else
	{
		sprintf_s(Buff, 32, "-%02.0f:%02.0f:%02.0f", floor(angle2 / 3600.0), floor(fmod(angle2, 3600.0) / 60.0), fmod(angle2, 60.0));
	}
}

bool AGOP::GetInertialLandmarkVector(double lat, double lng, double alt, double GMT, bool isEarth, VECTOR3 &R_LMK)
{
	//Returns landmark position in inertial (ECI or MCI) coordinates

	VECTOR3 R_L_equ;
	if (isEarth)
	{
		R_L_equ = OrbMech::r_from_latlong(lat, lng + OrbMech::w_Earth*GMT, OrbMech::R_Earth + alt);

		int err = pRTCC->ELVCNV(R_L_equ, GMT, 1, RTCC_COORDINATES_ECT, RTCC_COORDINATES_ECI, R_LMK);
		if (err) return true;
	}
	else
	{
		R_L_equ = OrbMech::r_from_latlong(lat, lng, pRTCC->BZLAND.rad[0] + alt);

		int err = pRTCC->ELVCNV(R_L_equ, GMT, 1, RTCC_COORDINATES_MCT, RTCC_COORDINATES_MCI, R_LMK);
		if (err) return true;
	}

	return false;
}

void AGOP::CSMHGAngles(VECTOR3 R, MATRIX3 SMNB, MATRIX3 REFSMMAT, double &pitch, double &yaw)
{
	//R = vector from CM to ground station

	VECTOR3 U_X, U_Y, U_Z, RP, U_RP, U_R;
	double x;

	R = mul(SMNB, mul(REFSMMAT, R)); //R in CM body axes

	U_R = unit(R);
	U_X = _V(1, 0, 0);
	U_Y = _V(0, 1, 0);
	U_Z = _V(0, 0, 1);

	RP = R - U_Z * dotp(R, U_Z);
	U_RP = unit(RP);

	yaw = acos(dotp(U_RP, U_X));
	x = dotp(U_RP, U_Y);
	if (x < 0)
	{
		yaw = PI2 - yaw;
	}
	pitch = acos(dotp(U_R, U_Z)) - PI05;
}

void AGOP::LMSteerableAngles(VECTOR3 R, MATRIX3 SMNB, MATRIX3 REFSMMAT, double &pitch, double &yaw)
{
	//R = vector from LM to ground station

	MATRIX3 NBSA;
	VECTOR3 U_X, U_Y, U_Z, RP, U_RP, U_R, X, Z, YAW;
	double Y;

	NBSA = _M(cos(45.0*RAD), sin(45.0*RAD), 0, -sin(45.0*RAD), cos(45.0*RAD), 0, 0, 0, 1);
	R = mul(NBSA, mul(SMNB, mul(REFSMMAT, R))); //R in LM body axes

	U_R = unit(R);
	U_X = _V(1, 0, 0);
	U_Y = _V(0, 1, 0);
	U_Z = _V(0, 0, 1);

	RP = R - U_Y * dotp(R, U_Y);
	U_RP = unit(RP);

	X = crossp(U_Z, U_RP);
	pitch = asin(length(X)*OrbMech::sign(dotp(X, U_Y)));

	Y = dotp(U_RP, U_Z);
	if (Y < 0.0)
	{
		pitch = PI - pitch;
	}

	Z = crossp(U_R, U_RP);
	YAW = U_X * cos(pitch) - U_Z * sin(pitch);
	yaw = asin(length(Z)*OrbMech::sign(dotp(YAW, Z)));
}

void AGOP::RRAngles(VECTOR3 R, MATRIX3 SMNB, MATRIX3 REFSMMAT, double &trunnion, double &shaft)
{
	VECTOR3 u_D, u_P;

	//Pointing vector in body axes
	u_D = mul(SMNB, mul(REFSMMAT, unit(R)));

	u_P = unit(_V(u_D.x, 0, u_D.z));

	trunnion = -asin(u_D.y);
	if (trunnion < 0)
	{
		trunnion += PI2;
	}

	//Displayed trunnion the reverse of CDU trunnion
	trunnion = PI2 - trunnion;

	shaft = atan2(u_P.x, u_P.z);
	if (shaft < 0)
	{
		shaft += PI2;
	}
}

VECTOR3 AGOP::CSMIMUtoLMIMUAngles(MATRIX3 CSM_REFSMMAT, MATRIX3 LM_REFSMMAT, VECTOR3 CSMIMUAngles, double DockingAngle)
{
	MATRIX3 M_NBCSM_NBLM, M_SMCSM_NBCSM, M_BRCS_SMCSM, M_BRCS_SMLM, M_BRCS_NBLM;

	M_NBCSM_NBLM = OrbMech::CSMBodyToLMBody(DockingAngle);
	M_SMCSM_NBCSM = OrbMech::CALCSMSC(CSMIMUAngles);
	M_BRCS_SMCSM = CSM_REFSMMAT;
	M_BRCS_SMLM = LM_REFSMMAT;
	M_BRCS_NBLM = mul(mul(M_NBCSM_NBLM, M_SMCSM_NBCSM), M_BRCS_SMCSM);

	return OrbMech::CALCGAR(M_BRCS_SMLM, M_BRCS_NBLM);
}

VECTOR3 AGOP::LMIMUtoCMIMUAngles(MATRIX3 CSM_REFSMMAT, MATRIX3 LM_REFSMMAT, VECTOR3 LMIMUAngles, double DockingAngle)
{
	MATRIX3 M_NBCSM_NBLM, M_SMLM_NBLM, M_BRCS_SMCSM, M_BRCS_SMLM, M_BRCS_NBCSM;

	M_NBCSM_NBLM = OrbMech::CSMBodyToLMBody(DockingAngle);
	M_SMLM_NBLM = OrbMech::CALCSMSC(LMIMUAngles);
	M_BRCS_SMCSM = CSM_REFSMMAT;
	M_BRCS_SMLM = LM_REFSMMAT;
	M_BRCS_NBCSM = mul(OrbMech::tmat(M_NBCSM_NBLM), mul(M_SMLM_NBLM, M_BRCS_SMLM));

	return OrbMech::CALCGAR(M_BRCS_SMCSM, M_BRCS_NBCSM);
}

MATRIX3 AGOP::LVLHAttitude(VECTOR3 LVLHAtt, VECTOR3 R, VECTOR3 V)
{
	double SINP, SINY, SINR, COSP, COSY, COSR;
	SINP = sin(LVLHAtt.y);
	SINY = sin(LVLHAtt.z);
	SINR = sin(LVLHAtt.x);
	COSP = cos(LVLHAtt.y);
	COSY = cos(LVLHAtt.z);
	COSR = cos(LVLHAtt.x);

	VECTOR3 Z_P, Y_P, X_P;
	Z_P = -unit(R);
	Y_P = -unit(crossp(R, V));
	X_P = crossp(Y_P, Z_P);

	double AL, BE, a1, a2, a3, b1, b2, b3, c1, c2, c3;
	AL = SINP * SINR;
	BE = SINP * COSR;
	a1 = COSY * COSP;
	a2 = SINY * COSP;
	a3 = -SINP;
	b1 = AL * COSY - SINY * COSR;
	b2 = AL * SINY + COSY * COSR;
	b3 = COSP * SINR;
	c1 = BE * COSY + SINY * SINR;
	c2 = BE * SINY - COSY * SINR;
	c3 = COSP * COSR;

	VECTOR3 X_B, Y_B, Z_B;
	X_B = X_P * a1 + Y_P * a2 + Z_P * a3;
	Y_B = X_P * b1 + Y_P * b2 + Z_P * b3;
	Z_B = X_P * c1 + Y_P * c2 + Z_P * c3;

	return _M(X_B.x, X_B.y, X_B.z, Y_B.x, Y_B.y, Y_B.z, Z_B.x, Z_B.y, Z_B.z);
}

MATRIX3 AGOP::ThreeAxisPointing(VECTOR3 SCAXIS, VECTOR3 U_LOS, VECTOR3 R, VECTOR3 V, double OMICRON)
{
	//INPUT:
	//SCAXIS: Pointing direction in navigation base coordinates
	//U_LOS: Pointing direction in inertial coordinates
	//R: position vector in inertial coordinates
	//V: velocity vector in inertial coordinates
	//OMICRON: essentially LVLH roll angle

	//OUTPUT:
	//Navigation base orientation matrix

	//Math from Artemis 72 code

	VECTOR3 unitY, UTSB, UTSBP, UTSAP, POINTVSM, UTSA, UTUYP, UTUZP, UTUY, UTUZ;

	unitY = _V(0, 1, 0);

	UTSB = U_LOS;
	UTSBP = SCAXIS;
	UTSAP = unit(crossp(UTSBP, unitY)); //Error?
	POINTVSM = unit(crossp(V, R));
	POINTVSM = unit(crossp(UTSB, POINTVSM));

	UTSA = POINTVSM * cos(OMICRON) + unit(crossp(UTSB, POINTVSM))*sin(OMICRON);
	UTUYP = unit(crossp(UTSAP, UTSBP));
	UTUZP = crossp(UTSAP, UTUYP);
	UTUY = unit(crossp(UTSA, UTSB));
	UTUZ = crossp(UTSA, UTUY);

	return OrbMech::AXISGEN(UTUYP, UTUZP, UTUY, UTUZ);
}

VECTOR3 AGOP::GetBodyFixedHGAVector(double pitch, double yaw) const
{
	return _V(cos(yaw)*cos(pitch), sin(yaw)*cos(pitch), -sin(pitch));
}

VECTOR3 AGOP::GetBodyFixedSteerableAntennaVector(double pitch, double yaw) const
{
	MATRIX3 NBSA;

	NBSA = _M(cos(45.0*RAD), sin(45.0*RAD), 0, -sin(45.0*RAD), cos(45.0*RAD), 0, 0, 0, 1);

	return tmul(NBSA, _V(cos(yaw)*cos(pitch), sin(yaw)*cos(pitch), -sin(pitch)));
}

VECTOR3 AGOP::GetBodyFixedRRVector(double trunnion, double shaft) const
{
	//Displayed trunnion the reverse of CDU trunnion
	trunnion = PI2 - trunnion;

	return _V(sin(shaft)*cos(trunnion), -sin(trunnion), cos(shaft)*cos(trunnion));
}

VECTOR3 AGOP::GetStarUnitVector(const AGOPInputs &in, unsigned star)
{
	if (star <= 400U)
	{
		//From table
		return in.startable[star - 1];
	}

	//Manual input
	return OrbMech::r_from_latlong(in.StarDeclination, in.StarRightAscension);
}

VECTOR3 AGOP::GetNBUnitVectorFromInstrument(const AGOPInputs &in, int set) const
{
	if (in.Instrument == 0)
	{
		// Sextant
		return OrbMech::SXTNB(in.SextantTrunnionAngles[set], in.SextantShaftAngles[set]);
	}
	else if (in.Instrument == 1)
	{
		// LM COAS
		return GetLMCOASVector(in.COASElevationAngle[0], in.COASPositionAngle[0], in.LMCOASAxis);
	}
	else if (in.Instrument == 2)
	{
		// AOT

		double AZ, EL;

		GetAOTNBAngle(in.AOTDetent, AZ, EL);
		return GetAOTNBVector(EL, AZ, in.AOTReticleAngle[0], in.AOTSpiraleAngle[0], in.AOTLineID[0]);
	}
	else
	{
		// CSM COAS
		return GetCSMCOASVector(in.COASElevationAngle[0], in.COASPositionAngle[0]);
	}
}

VECTOR3 AGOP::GetSextantVector(double TRN, double SFT) const
{
	//In navigation base coordinates
	return OrbMech::SXTNB(TRN, SFT);
}

VECTOR3 AGOP::GetCSMCOASVector(double SPA, double SXP) const
{
	//In navigation base coordinates
	return unit(_V(cos(SPA)*cos(SXP), sin(SXP), sin(SPA)*cos(SXP)));
}

VECTOR3 AGOP::GetLMCOASVector(double EL, double SXP, bool IsZAxis) const
{
	//In navigation base coordinates
	if (IsZAxis)
	{
		return unit(_V(sin(SXP), -sin(EL)*cos(SXP), cos(EL)*cos(SXP)));
	}

	//X-axis
	return unit(_V(cos(EL)*cos(SXP), sin(SXP), sin(EL)*cos(SXP)));
}

VECTOR3 AGOP::GetAOTNBVector(double EL, double AZ, double ReticleAngle, double SpiraleAngle, int axis) const
{
	VECTOR3 u_OAN, u_XPN_apo, u_YPN_apo, u_XPN, u_YPN;
	double RN;

	u_OAN = _V(sin(EL), cos(EL)*sin(AZ), cos(EL)*cos(AZ));
	u_YPN_apo = _V(0.0, cos(AZ), -sin(AZ));
	u_XPN_apo = crossp(u_YPN_apo, u_OAN);
	RN = 0.0 - AZ;
	u_XPN = u_XPN_apo * cos(RN) + u_YPN_apo * sin(RN);
	u_YPN = -u_XPN_apo * sin(RN) + u_YPN_apo * cos(RN);

	if (ReticleAngle == 0.0 && SpiraleAngle == 0.0)
	{
		// P52 technique
		
		return unit(crossp(u_XPN, u_YPN));
	}
	else
	{
		// P57 technique

		VECTOR3 u_YPN_aapo;
		double YROT, SROT, SEP;

		//1 = +Y-axis, 2 = +X-axis, 3 = -Y-axis, 4 = -X-axis
		switch (axis)
		{
		case 2: //+X
			YROT = ReticleAngle + 270.0*RAD;
			break;
		case 3: //-Y
			YROT = ReticleAngle + 180.0*RAD;
			break;
		case 4: //-X
			YROT = ReticleAngle + 90.0*RAD;
			break;
		default: //+Y
			YROT = ReticleAngle;
			break;
		}

		SROT = SpiraleAngle;

		SEP = (PI2 + SROT - YROT) / 12.0;

		u_YPN_aapo = -u_XPN * sin(YROT) + u_YPN * cos(YROT);
		return u_OAN * cos(SEP) + crossp(u_YPN_aapo, u_OAN)*sin(SEP);
	}
}

VECTOR3 AGOP::VectorPointingToHorizon(EphemerisData sv, VECTOR3 plane, bool sol) const
{
	//Vector pointíng to Earth
	VECTOR3 e;
	double R_E, alpha;

	e = unit(-sv.R);

	if (sv.RBI == BODY_EARTH)
	{
		R_E = OrbMech::R_Earth;
	}
	else
	{
		R_E = pRTCC->BZLAND.rad[0];
	}

	alpha = asin(R_E / length(sv.R));

	if (sol == false)
	{
		alpha = -alpha;
	}

	return OrbMech::RotateVector(plane, alpha, e);
}

MATRIX3 AGOP::LS_REFSMMAT(VECTOR3 R_LS, VECTOR3 R_CSM, VECTOR3 V_CSM) const
{
	VECTOR3 X_SM, Y_SM, Z_SM;

	X_SM = unit(R_LS);
	Z_SM = unit(crossp(crossp(R_CSM, V_CSM), X_SM));
	Y_SM = unit(crossp(Z_SM, X_SM));

	return _M(X_SM.x, X_SM.y, X_SM.z, Y_SM.x, Y_SM.y, Y_SM.z, Z_SM.x, Z_SM.y, Z_SM.z);
}

bool AGOP::InstrumentLimitCheck(const AGOPInputs &in, VECTOR3 u_NB) const
{
	VECTOR3 u_LOS;

	if (in.Instrument == 0)
	{
		//Sextant
		u_LOS = mul(OrbMech::SBNBMatrix(), _V(0, 0, 1));

		if (acos(dotp(u_NB, u_LOS)) < 38.0*RAD) return true;
	}
	else if (in.Instrument == 1)
	{
		//LM COAS

		if (in.LMCOASAxis)
		{
			//Z-axis
			//Within 5 degrees of Y-Z plane?
			if (abs(u_NB.x) < cos(85.0*RAD))
			{
				//In BSS pitch limits (-10° to +70.0°)?
				if (u_NB.y < 0.0)
				{
					if (acos(u_NB.z) < 70.0*RAD) return true;
				}
				else
				{
					if (acos(u_NB.z) < 10.0*RAD) return true;
				}
			}
		}
		else
		{
			//X-axis

			//Within 5 degrees of X-Z plane?
			if (asin(abs(u_NB.y)) < 5.0*RAD)
			{
				//In BSS pitch limits (-5° to +35.0°)?
				if (u_NB.z < 0.0)
				{
					if (acos(u_NB.x) <= 5.0*RAD) return true;
				}
				else
				{
					if (acos(u_NB.x) <= 35.0*RAD) return true;
				}
			}
		}
	}
	else if (in.Instrument == 2)
	{
		//AOT
		double AZ, EL;

		GetAOTNBAngle(in.AOTDetent, AZ, EL);
		u_LOS = OrbMech::AOTNavigationBase(AZ, EL);

		if (acos(dotp(u_NB, u_LOS)) < 30.0*RAD) return true;
	}
	else
	{
		//CSM COAS

		//Within 5 degrees of X-Z plane?
		if (asin(abs(u_NB.y)) < 5.0*RAD)
		{
			//In BSS pitch limits (-15° to +36.5°)?
			if (u_NB.z < 0.0)
			{
				if (acos(u_NB.x) <= 15.0*RAD) return true;
			}
			else
			{
				if (acos(u_NB.x) <= 36.5*RAD) return true;
			}
		}
	}

	return false;
}

void AGOP::InstrumentAngles(VECTOR3 u_NB, int Instrument, int AOTDetent, bool LMCOASAxis, double &pitch, double &yaw) const
{
	if (Instrument == 0)
	{
		SextantAngles(u_NB, pitch, yaw);
	}
	else if (Instrument == 1)
	{
		LMCOASAngles(LMCOASAxis, u_NB, pitch, yaw);
	}
	else if (Instrument == 2)
	{
		AOTAngles(AOTDetent, u_NB, pitch, yaw);
	}
	else
	{
		CSMCOASAngles(u_NB, pitch, yaw);
	}
}

void AGOP::SextantAngles(VECTOR3 u_NB, double &TA, double &SA) const
{
	VECTOR3 X_SB, Y_SB, Z_SB, U_TPA, u_SB;
	double sinSA, cosSA;

	u_SB = tmul(OrbMech::SBNBMatrix(), u_NB);

	X_SB = _V(1.0, 0.0, 0.0);
	Y_SB = _V(0.0, 1.0, 0.0);
	Z_SB = _V(0.0, 0.0, 1.0);

	U_TPA = unit(crossp(Z_SB, u_SB));
	sinSA = dotp(U_TPA, -X_SB);
	cosSA = dotp(U_TPA, Y_SB);
	SA = OrbMech::atan3(sinSA, cosSA);
	TA = acos(dotp(Z_SB, u_SB));
}

void AGOP::AOTAngles(int Detent, VECTOR3 u_NB, double &YROT, double &SROT) const
{
	VECTOR3 u_OAN, UNITX, TS2, TS4;
	double AZ, EL, C1, theta, C2;

	GetAOTNBAngle(Detent, AZ, EL);

	u_OAN = _V(sin(EL), cos(EL)*sin(AZ), cos(EL)*cos(AZ));
	C1 = dotp(u_OAN, u_NB);

	UNITX = _V(1, 0, 0);
	TS2 = unit(crossp(u_OAN, UNITX));
	TS4 = unit(crossp(u_OAN, u_NB));
	theta = acos(dotp(TS4, TS2));
	C2 = dotp(TS4, unit(crossp(u_OAN, TS2)));
	if (C2 < 0.0)
	{
		theta = PI2 - theta;
	}
	YROT = PI2 + theta + AZ;
	while (YROT >= PI2)
	{
		YROT -= PI2;
	}
	SROT = YROT + 12.0*acos(C1);
	while (SROT >= PI2)
	{
		SROT -= PI2;
	}
}

void AGOP::CSMCOASAngles(VECTOR3 u_NB, double &SPA, double &SXP) const
{
	SPA = -atan(u_NB.z / u_NB.x);
	SXP = asin(u_NB.y);
}

void AGOP::LMCOASAngles(bool Axis, VECTOR3 u_NB, double &EL, double &SXP) const
{
	double EPS, GAM, ALP, R, SCV;

	if (Axis)
	{
		//Z-axis
		double HYP;

		EPS = acos(u_NB.z);
		GAM = acos(u_NB.x);
		HYP = sqrt(u_NB.x*u_NB.x + u_NB.y*u_NB.y);
		ALP = atan(HYP / u_NB.z);
		SCV = PI05 - abs(asin(sin(GAM)*sin(ALP) / sin(EPS)));
		R = u_NB.x*abs(asin(sin(SCV)*sin(EPS)) / u_NB.x);
		if (u_NB.x < 0)
		{
			SXP = -abs(R);
		}
		else
		{
			SXP = abs(R);
		}
		EL = u_NB.y*abs(acos(cos(EPS) / cos(SXP)) / u_NB.y);
		if (u_NB.y < 0.0)
		{
			EL = abs(EL);
		}
		else
		{
			EL = -abs(EL);
		}
		EL -= 30.0*RAD;
	}
	else
	{
		//X-axis
		double ARG1, ARG2, ARG3;

		EPS = acos(u_NB.x);
		GAM = acos(u_NB.z);
		ALP = atan(u_NB.y / u_NB.x);
		ARG1 = sin(GAM)*sin(ALP) / sin(EPS);
		SCV = PI05 - abs(asin(ARG1));
		ARG2 = sin(SCV)*sin(EPS);
		R = u_NB.z*abs(asin(ARG2) / u_NB.z);
		ARG3 = cos(EPS) / cos(R);
		SXP = u_NB.y*abs(acos(ARG3) / u_NB.y);
		if (u_NB.y < 0)
		{
			SXP = -abs(SXP);
		}
		else
		{
			SXP = abs(SXP);
		}
		EL = R;
		if (u_NB.z < 0)
		{
			EL = abs(EL);
		}
		else
		{
			EL = -abs(EL);
		}
	}
}

//Calculates station vector in ECT coordinates
VECTOR3 Station_ECT(double GMT, double R_E_sin_lat, double R_E_cos_lat, double stat_lng)
{
	double lng = stat_lng + OrbMech::w_Earth*GMT;
	return _V(R_E_cos_lat*cos(lng), R_E_cos_lat*sin(lng), R_E_sin_lat);
}

int AGOP::FindLandmarkAOS(AGOPOutputs &out, StationData station, double GMT_start, double ElevationAngle, double &GMT_elev)
{
	//GMT assumed to be 0° elevation crossing time
	EphemerisData sv;
	VECTOR3 R_S, N, rho;
	double TL, TR, GMT, sinang, sinang_desired, eps;
	unsigned i, j, limit;
	bool err;

	sinang_desired = sin(ElevationAngle);
	i = 0U;
	j = 0U;
	limit = 100U;
	eps = 1.0;
	if (ephemeris.Header.CSI == RTCC_COORDINATES_MCT) R_S = _V(station.R_E_cos_lat*station.cos_lng, station.R_E_cos_lat*station.sin_lng, station.R_E_sin_lat);

	//Find first state vector after GMT

	while (GMT_start <= ephemeris.table[i].GMT)
	{
		i++;

		if (i >= ephemeris.table.size())
		{
			//Error
			return 1;
		}
	}

	TL = GMT_start;
	TR = ephemeris.table[i].GMT;

	do
	{
		GMT = ephemeris.table[i].GMT;

		err = Interpolation(GMT, sv);
		if (err)
		{
			WriteError(out, 4);
			return 4;
		}

		if (ephemeris.Header.CSI == RTCC_COORDINATES_ECT) R_S = Station_ECT(GMT, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
		OrbMech::EMXINGElev(sv.R, R_S, N, rho, sinang);

		if (sinang > sinang_desired)
		{
			//Past target
			TR = GMT;
			break;
		}
		else
		{
			//Before target
			TL = GMT;
		}

		i++;

	} while (i < ephemeris.table.size());

	if (i >= ephemeris.table.size())
	{
		//Error
		return 1;
	}

	//Interval halfing
	while (TR - TL > eps)
	{
		//Try at midpoint
		GMT = (TL + TR) / 2.0;

		err = Interpolation(GMT, sv);
		if (err)
		{
			WriteError(out, 4);
			return 4;
		}

		if (ephemeris.Header.CSI == RTCC_COORDINATES_ECT) R_S = Station_ECT(GMT, station.R_E_sin_lat, station.R_E_cos_lat, station.lng);
		OrbMech::EMXINGElev(sv.R, R_S, N, rho, sinang);

		//Test condition
		if (sinang > sinang_desired)
		{
			//Condition exists at GMT, set as new right limit
			TR = GMT;
		}
		else
		{
			//Condition does not exist at GMT, new left limit
			TL = GMT;
		}
		j++;
		if (j >= limit)
		{
			break;
		}
	}

	if (j == limit)
	{
		WriteError(out, 4);
		return 4;
	}

	GMT_elev = (TL + TR) / 2.0;
	return 0;
}

bool AGOP::Interpolation(double GMT, EphemerisData &sv)
{
	ELVCTRInputTable intab;
	ELVCTROutputTable2 outtab;

	intab.GMT = GMT;

	pRTCC->ELVCTR(intab, outtab, ephemeris, mantimes);

	if (outtab.ErrorCode > 2)
	{
		return true;
	}

	sv.R = outtab.SV.R;
	sv.V = outtab.SV.V;
	sv.GMT = outtab.SV.GMT;
	sv.RBI = ephemeris.Header.CSI == 0 ? BODY_EARTH : BODY_MOON;

	return false;
}

EphemerisData AGOP::SingleStateVector()
{
	EphemerisData sv;

	sv.R = ephemeris.table[0].R;
	sv.V = ephemeris.table[0].V;
	sv.GMT = ephemeris.table[0].GMT;
	sv.RBI = ephemeris.Header.CSI == 0 ? BODY_EARTH : BODY_MOON;

	return sv;
}

MATRIX3 AGOP::BRCStoNBMatrix(const AGOPInputs &in, int set) const
{
	//Returns the BRCS to NB matrix for the NB of the vehicle with the instrument

	MATRIX3 M_BRCS_SM, M_SM_NB, M_BRCS_NB;
	bool conv;

	//Does the attitude have to be converged from one vehicle to the other?
	conv = PointInstrumentOfOtherVehicle(in);

	if (in.AttIsCSM)
	{
		M_BRCS_SM = in.CSM_REFSMMAT;
	}
	else
	{
		M_BRCS_SM = in.LM_REFSMMAT;
	}
	M_SM_NB = OrbMech::CALCSMSC(in.IMUAttitude[0]);

	//BRCS to NB (of vehicle with the attitude!) matrix
	M_BRCS_NB = mul(M_SM_NB, M_BRCS_SM);

	if (conv)
	{
		MATRIX3 M_NBCSM_NBLM;

		M_NBCSM_NBLM = OrbMech::CSMBodyToLMBody(in.DockingAngle);

		if (in.AttIsCSM)
		{
			M_BRCS_NB = mul(M_NBCSM_NBLM, M_BRCS_NB);
		}
		else
		{
			M_BRCS_NB = mul(OrbMech::tmat(M_NBCSM_NBLM), M_BRCS_NB);
		}
	}

	return M_BRCS_NB;
}

void AGOP::GetAttitudeMatrix(const AGOPInputs &in, int set, VECTOR3 &GA, MATRIX3 &MAT) const
{
	// Stable member to navigation base

	if (in.AttIsCSM)
	{
		GA = in.IMUAttitude[set];
	}
	else
	{
		if (in.AttIsFDAI)
		{
			GA = pRTCC->EMMGFDAI(in.IMUAttitude[set], false);
		}
		else
		{
			GA = in.IMUAttitude[set];
		}
	}

	MAT = OrbMech::CALCSMSC(GA);
}

bool AGOP::PointInstrumentOfOtherVehicle(const AGOPInputs &in) const
{
	bool conv = false;

	if (in.Instrument == 0 || in.Instrument == 3)
	{
		//CSM
		if (in.AttIsCSM == false)
		{
			conv = true;
		}
	}
	else
	{
		//LM
		if (in.AttIsCSM == true)
		{
			conv = true;
		}
	}

	return conv;
}

void AGOP::GetAOTNBAngle(int Detent, double &AZ, double &EL) const
{
	AZ = pRTCC->SystemParameters.MDGTCD[Detent];
	EL = pRTCC->SystemParameters.MDGETA[Detent];
}