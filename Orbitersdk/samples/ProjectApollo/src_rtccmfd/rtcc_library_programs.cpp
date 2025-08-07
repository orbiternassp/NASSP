/****************************************************************************
This file is part of Project Apollo - NASSP

Real-Time Computer Complex (RTCC) Library Programs

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

#include "rtcc.h"

//TBD: BLMDFQ etc.

//Ephemeris Fetch Routine
int RTCC::ELFECH(double GMT, int L, EphemerisData &SV)
{
	EphemerisDataTable2 EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;
	int err = ELFECH(GMT, 1, 1, L, EPHEM, MANTIMES, LUNSTAY);
	if (err == 0)
	{
		SV.R = EPHEM.table[0].R;
		SV.V = EPHEM.table[0].V;
		SV.GMT = EPHEM.table[0].GMT;
		SV.RBI = BODY_EARTH;
		RotateSVToSOI(SV); //Probably shouldn't be here, but it's convenient
	}
	return err;
}

int RTCC::ELFECH(double GMT, unsigned vec_tot, unsigned vec_bef, int L, EphemerisDataTable2 &EPHEM, ManeuverTimesTable &MANTIMES, LunarStayTimesTable &LUNSTAY)
{
	OrbitEphemerisTable *maintable;
	unsigned LO, HI, temp;
	double GMTtemp;

	if (L == RTCC_MPT_CSM)
	{
		maintable = &EZEPH1;
	}
	else
	{
		maintable = &EZEPH2;
	}

	if (maintable->EPHEM.table.size() == 0)
	{
		return 1;
	}

	MANTIMES = maintable->MANTIMES;
	LUNSTAY = maintable->LUNRSTAY;

	//GMT is before first SV in ephemeris, error
	if (GMT < maintable->EPHEM.table.front().GMT)
	{
		return 1;
	}
	//GMT is beyond last SV in ephemeris, error return if more than one SV is requested. Otherwise the last SV will be returned
	if (vec_tot > 1 && GMT > maintable->EPHEM.table.back().GMT)
	{
		return 1;
	}

	//Clear output ephemeris table
	EPHEM.table.clear();

	//Special code for data beyond the ephemeris range, only is used for 1 SV to be returned
	if (GMT > maintable->EPHEM.table.back().GMT)
	{
		EPHEM.table.push_back(maintable->EPHEM.table.back());
	}
	//All other cases
	else
	{
		LO = 0;
		HI = maintable->EPHEM.table.size() - 1;

		while (HI - LO > 1)
		{
			temp = (LO + HI) / 2;
			GMTtemp = maintable->EPHEM.table[temp].GMT;
			if (GMT > GMTtemp)
			{
				LO = temp;
			}
			else
			{
				HI = temp;
			}
		}

		if (maintable->EPHEM.table[LO].GMT < GMT)
		{
			LO++;
		}

		if (LO < vec_bef)
		{
			LO = 0;
		}
		else
		{
			LO = LO - vec_bef;
		}

		HI = LO + vec_tot;

		if (HI >= maintable->EPHEM.table.size())
		{
			HI = maintable->EPHEM.table.size() - 1;
		}

		auto first = maintable->EPHEM.table.cbegin() + LO;
		auto last = maintable->EPHEM.table.cbegin() + HI;

		EPHEM.table.assign(first, last);
	}

	MANTIMES = maintable->MANTIMES;
	EPHEM.Header.TUP = maintable->EPHEM.Header.TUP;
	EPHEM.Header.VEH = L;
	EPHEM.Header.NumVec = EPHEM.table.size();
	EPHEM.Header.Offset = 0;
	EPHEM.Header.CSI = 0;
	if (EPHEM.Header.NumVec > 0)
	{
		EPHEM.Header.TL = EPHEM.table.front().GMT;
		EPHEM.Header.TR = EPHEM.table.back().GMT;
	}
	else
	{
		EPHEM.Header.TL = maintable->EPHEM.Header.TL;
		EPHEM.Header.TR = maintable->EPHEM.Header.TR;
		return 1;
	}

	return 0;
}

void RTCC::ELGLCV(double lat, double lng, VECTOR3 &out, double rad)
{
	double R_M;

	if (rad == 0.0)
	{
		R_M = SystemParameters.MCSMLR;
	}
	else
	{
		R_M = rad;
	}
	out = _V(cos(lat)*cos(lng), cos(lat)*sin(lng), sin(lat))*R_M;
}

void RTCC::ELGLCV(double lat, double lng, MATRIX3 &out, double rad)
{
	VECTOR3 X, Y, Z;

	X = _V(cos(lat)*cos(lng), cos(lat)*sin(lng), sin(lat));
	Y = unit(_V(-cos(lat)*sin(lng), cos(lat)*sin(lat), 0.0));
	Z = crossp(X, Y);
	out = _M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z);
}

//Vector Count Routine
int RTCC::ELNMVC(double &TL, double TR, int L, unsigned &NumVec, int &TUP)
{
	//Error returns:
	//0 = all ok
	//8 = on if TL < ephemeris begin time
	//16 = on if TR > ephemeris end time (TBD)
	//32 = On if requested vehicle is not active
	//64 = On if requested ephemeris is not being carried
	//128 = On if requested interval does not overlap ephemeris span
	int err = 0;

	//Obtain pointer to ephemeris table name
	OrbitEphemerisTable *tab;
	if (L == 1)
	{
		tab = &EZEPH1;
	}
	else
	{
		tab = &EZEPH2;
	}

	//Vehicle active? TBD: This should probably check some flag instead
	if (tab->EPHEM.table.size() == 0)
	{
		err = 32;
		goto RTCC_ELNMVC_2B;
	}
	//Table being carried?
	TUP = abs(tab->EPHEM.Header.TUP);
	if (TUP == 0)
	{
		err = 64;
		goto RTCC_ELNMVC_2B;
	}

	//Read-lock ephemeris
	tab->EPHEM.Header.TUP = -tab->EPHEM.Header.TUP;

	//Left request time < ephemeris begin?
	if (TL < tab->EPHEM.Header.TL)
	{
		//Right request time < ephemeris begin?
		if (TR < tab->EPHEM.Header.TL)
		{
			//Time outside ephemeris error
			err = 128;
			goto RTCC_ELNMVC_2B;
		}
		//Store ephemeris begin in user TL. Set 'TL adjusted' bit
		TL = tab->EPHEM.Header.TL;
		err = 8;
	}
	//Left request time > ephemeris end?
	if (TL > tab->EPHEM.Header.TR)
	{
		//Time outside ephemeris error
		err = 128;
		goto RTCC_ELNMVC_2B;
	}
	//Set up counters and flags to find location of TL in ephemeris table
	double TREQ, T_Mid;
	unsigned LB, UB, Mid;
	bool firstpass = true;
	LB = 0;
	UB = tab->EPHEM.table.size() - 1;
	TREQ = TL;
RTCC_ELNMVC_1B:
	//Is one vector left?
	if (UB - LB <= 1)
	{
		goto RTCC_ELNMVC_1D;
	}
	//Compute midpoint of ephemeris
	Mid = (LB + UB) / 2;
	T_Mid = tab->EPHEM.table[Mid].GMT;
	//TREQ < T midpoint?
	if (TREQ < T_Mid)
	{
		//Upper bound = midpoint
		UB = Mid;
		goto RTCC_ELNMVC_1B;
	}
	//TREQ > T midpoint?
	if (TREQ > T_Mid)
	{
		//Lower bound = midpoint
		LB = Mid;
		goto RTCC_ELNMVC_1B;
	}
	LB = Mid;
	if (firstpass)
	{
		LB++;
	}
	goto RTCC_ELNMVC_2A;
RTCC_ELNMVC_1D:
	LB++;
RTCC_ELNMVC_2A:
	if (firstpass)
	{
		//Set up search for TR
		NumVec = LB;
		LB = NumVec;
		UB = tab->EPHEM.table.size() - 1;
		TREQ = TR;
		firstpass = false;
		goto RTCC_ELNMVC_1B;
	}
	NumVec = LB - NumVec + 2;
RTCC_ELNMVC_2B:
	tab->EPHEM.Header.TUP = -tab->EPHEM.Header.TUP;
	return err;
}

//Vector interpolation routine
int RTCC::ELVARY(EphemerisDataTable2 &EPH, unsigned ORER, double GMT, bool EXTRAP, EphemerisData2 &sv_out, unsigned &ORER_out)
{
	EphemerisData2 RES;
	VECTOR3 TERM1, TERM2;
	double TERM3;
	unsigned DESLEF, DESRI;
	unsigned i = EPH.Header.Offset;
	int ERR = 0;
	//Ephemeris too small
	if (EPH.Header.NumVec < 2)
	{
		return 128;
	}
	//Requested order too high
	if (ORER > 8)
	{
		return 64;
	}
	if (EPH.Header.NumVec > ORER)
	{
		//Store Order(?)
	}
	else
	{
		ERR += 2;
		ORER = EPH.Header.NumVec - 1;
	}

	if (GMT < EPH.Header.TL)
	{
		if (EXTRAP == false) return 32;
		if (GMT < EPH.Header.TL - 4.0) { return 8; }
		else { ERR += 1; }
	}
	if (GMT > EPH.Header.TR)
	{
		if (EXTRAP == false) return 16;
		if (GMT > EPH.Header.TR + 4.0) { return 4; }
		else { ERR += 1; }
	}

	while (GMT > EPH.table[i].GMT)
	{
		i++;
	}

	//Direct hit
	if (GMT == EPH.table[i].GMT)
	{
		sv_out = EPH.table[i];
		return ERR;
	}

	if (ORER % 2)
	{
		DESLEF = DESRI = (ORER + 1) / 2;
	}
	else
	{
		DESLEF = ORER / 2 + 1;
		DESRI = ORER / 2;
	}

	if (i < DESLEF + EPH.Header.Offset)
	{
		i = EPH.Header.Offset;
	}
	else if (i > EPH.Header.Offset + EPH.Header.NumVec - DESRI)
	{
		i = EPH.Header.Offset + EPH.Header.NumVec - ORER - 1;
	}
	else
	{
		i = i - DESLEF;
	}

	for (unsigned j = 0; j < ORER + 1; j++)
	{
		TERM1 = EPH.table[i + j].R;
		TERM2 = EPH.table[i + j].V;
		TERM3 = 1.0;
		for (unsigned k = 0;k < ORER + 1;k++)
		{
			if (k != j)
			{
				TERM3 *= (GMT - EPH.table[i + k].GMT) / (EPH.table[i + j].GMT - EPH.table[i + k].GMT);
			}
		}
		RES.R += TERM1 * TERM3;
		RES.V += TERM2 * TERM3;
	}

	RES.GMT = GMT;

	sv_out = RES;
	ORER_out = ORER;

	return ERR;
}

//Generalized Coordinate Conversion Routine
int RTCC::ELVCNV(EphemerisData sv, int out, EphemerisData &sv_out)
{
	EphemerisData2 sv1, sv_out2;
	int in;

	sv1.R = sv.R;
	sv1.V = sv.V;
	sv1.GMT = sv.GMT;

	if (sv.RBI == BODY_EARTH)
	{
		in = 0;
	}
	else
	{
		in = 2;
	}

	int err = ELVCNV(sv1, in, out, sv_out2);

	if (err)
	{
		return err;
	}
	sv_out.R = sv_out2.R;
	sv_out.V = sv_out2.V;
	sv_out.GMT = sv_out2.GMT;
	if (out < 2) //Set to Earth for ECI and ECT, to Moon for MCI, MCT, EMP
	{
		sv_out.RBI = 0;
	}
	else
	{
		sv_out.RBI = 2;
	}
	return 0;
}

int RTCC::ELVCNV(EphemerisData2 sv, int in, int out, EphemerisData2 &sv_out)
{
	std::vector<EphemerisData2> inv;

	inv.push_back(sv);

	int err = ELVCNV(inv, in, out, inv);
	sv_out = inv[0];

	return err;
}

int RTCC::ELVCNV(VECTOR3 vec, double GMT, int type, int in, int out, VECTOR3 &vec_out)
{
	std::vector<VECTOR3> inv;

	inv.push_back(vec);

	int err = ELVCNV(inv, GMT, type, in, out, inv);
	vec_out = inv[0];

	return err;
}

int RTCC::ELVCNV(double GMT, int in, int out, MATRIX3 &Rot)
{
	//Rotation matrix from "in" coordinate system to "out" coordinate system

	std::vector<VECTOR3> vec_in, vec_out;

	vec_in.push_back(_V(1, 0, 0));
	vec_in.push_back(_V(0, 1, 0));
	vec_in.push_back(_V(0, 0, 1));

	int err = ELVCNV(vec_in, GMT, 0, in, out, vec_out);

	Rot = _M(vec_out[0].x, vec_out[1].x, vec_out[2].x, vec_out[0].y, vec_out[1].y, vec_out[2].y, vec_out[0].z, vec_out[1].z, vec_out[2].z);

	return err;
}

int RTCC::ELVCNV(std::vector<EphemerisData2> &svtab, int in, int out, std::vector<EphemerisData2> &svtab_out)
{
	int err = 0;

	svtab_out = svtab;

	std::vector<VECTOR3> vec2;

	vec2.resize(2);

	for (unsigned i = 0; i < svtab.size(); i++)
	{
		vec2[0] = svtab[i].R;
		vec2[1] = svtab[i].V;

		err = ELVCNV(vec2, svtab[i].GMT, -1, in, out, vec2);
		if (err) return err;

		svtab_out[i].R = vec2[0];
		svtab_out[i].V = vec2[1];
	}

	return 0;
}

int RTCC::ELVCNV(std::vector<VECTOR3> VECTORS, double GMT, int type, int in, int out, std::vector<VECTOR3> &OUTPUT)
{
	//VECTORS: table of input vector(s)
	//GMT: GMT of vector(s)
	//type: -1 = state vector, 0 = unit vector, 1 = position vector, 2 = velocity vector
	//in and out: 0 = ECI, 1 = ECT, 2 = MCI, 3 = MCT, 4 = EMP, 5 = MCTVR
	//OUTPUT: Table of output vector(s), may be the same as VECTORS

	OUTPUT = VECTORS;

	if (in == out)
	{
		return 0;
	}

	bool IsMCTVR = false;

	if (in == 5 || out == 5)
	{
		if (type != -1)
		{
			return 1;
		}

		if (in == 5)
		{
			in = RTCC_COORDINATES_MCT;
		}
		else
		{
			out = RTCC_COORDINATES_MCT;
		}
		IsMCTVR = true;
	}

	//Set up options
	int num_conv = 0;
	int conv[3]; //0 = ECI to MCI, 1 = MCI to ECI, 2 = MCI to MCT, 3 = MCT to MCI, 4 = ECI to ECT, 5 = ECT to ECI, 6 = MCI to EMP, 7 = EMP to MCI 
	if (in == RTCC_COORDINATES_ECI && out == RTCC_COORDINATES_ECT) //ECI to ECT
	{
		conv[0] = 4;
		num_conv = 1;
	}
	else if (in == RTCC_COORDINATES_ECI && out == RTCC_COORDINATES_MCI) //ECI to MCI
	{
		conv[0] = 0;
		num_conv = 1;
	}
	else if (in == RTCC_COORDINATES_ECI && out == RTCC_COORDINATES_MCT) //ECI to MCT
	{
		conv[0] = 0; conv[1] = 2;
		num_conv = 2;
	}
	else if (in == RTCC_COORDINATES_ECI && out == RTCC_COORDINATES_EMP) //ECI to EMP
	{
		conv[0] = 0; conv[1] = 6;
		num_conv = 2;
	}
	else if (in == RTCC_COORDINATES_ECT && out == RTCC_COORDINATES_ECI) //ECT to ECI
	{
		conv[0] = 5;
		num_conv = 1;
	}
	else if (in == RTCC_COORDINATES_ECT && out == RTCC_COORDINATES_MCI) //ECT to MCI
	{
		conv[0] = 5; conv[1] = 0;
		num_conv = 2;
	}
	else if (in == RTCC_COORDINATES_ECT && out == RTCC_COORDINATES_MCT) //ECT to MCT
	{
		conv[0] = 5; conv[1] = 0; conv[2] = 2;
		num_conv = 3;
	}
	else if (in == RTCC_COORDINATES_ECT && out == RTCC_COORDINATES_EMP) //ECT to EMP
	{
		conv[0] = 5; conv[1] = 0; conv[2] = 6;
		num_conv = 3;
	}
	else if (in == RTCC_COORDINATES_MCI && out == RTCC_COORDINATES_ECI) //MCI to ECI
	{
		conv[0] = 1;
		num_conv = 1;
	}
	else if (in == RTCC_COORDINATES_MCI && out == RTCC_COORDINATES_ECT) //MCI to ECT
	{
		conv[0] = 1; conv[1] = 4;
		num_conv = 2;
	}
	else if (in == RTCC_COORDINATES_MCI && out == RTCC_COORDINATES_MCT) //MCI to MCT
	{
		conv[0] = 2;
		num_conv = 1;
	}
	else if (in == RTCC_COORDINATES_MCI && out == RTCC_COORDINATES_EMP) //MCI to EMP
	{
		conv[0] = 6;
		num_conv = 1;
	}
	else if (in == RTCC_COORDINATES_MCT && out == RTCC_COORDINATES_ECI) //MCT to ECI
	{
		conv[0] = 3; conv[1] = 1;
		num_conv = 2;
	}
	else if (in == RTCC_COORDINATES_MCT && out == RTCC_COORDINATES_ECT) //MCT to ECT
	{
		conv[0] = 3; conv[1] = 1; conv[2] = 4;
		num_conv = 3;
	}
	else if (in == RTCC_COORDINATES_MCT && out == RTCC_COORDINATES_MCI) //MCT to MCI
	{
		conv[0] = 3;
		num_conv = 1;
	}
	else if (in == RTCC_COORDINATES_MCT && out == RTCC_COORDINATES_EMP) //MCT to EMP
	{
		conv[0] = 3; conv[1] = 6;
		num_conv = 2;
	}
	else if (in == RTCC_COORDINATES_EMP && out == RTCC_COORDINATES_ECI) //EMP to ECI
	{
		conv[0] = 7; conv[1] = 1;
		num_conv = 2;
	}
	else if (in == RTCC_COORDINATES_EMP && out == RTCC_COORDINATES_ECT) //EMP to ECT
	{
		conv[0] = 7; conv[1] = 1; conv[1] = 4;
		num_conv = 3;
	}
	else if (in == RTCC_COORDINATES_EMP && out == RTCC_COORDINATES_MCI) //EMP to MCI
	{
		conv[0] = 7;
		num_conv = 1;
	}
	else if (in == RTCC_COORDINATES_EMP && out == RTCC_COORDINATES_MCT) //EMP to MCT
	{
		conv[0] = 7; conv[1] = 2;
		num_conv = 2;
	}
	
	for (int i = 0; i < num_conv; i++)
	{
		if (conv[i] == 0 || conv[i] == 1)
		{
			if (type == 0)
			{
				//Nothing to do
			}
			else
			{
				VECTOR3 R_EM, V_EM, R_ES;
				if (PLEFEM(1, GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL))
				{
					return 1;
				}

				double sign;

				if (conv[i] == 0)
				{
					sign = -1.0;
				}
				else
				{
					sign = 1.0;
				}

				if (type == 1)
				{
					for (unsigned j = 0; j < OUTPUT.size(); j++)
					{
						OUTPUT[j] = OUTPUT[j] + R_EM * sign;
					}
				}
				else if (type == 2)
				{
					for (unsigned j = 0; j < OUTPUT.size(); j++)
					{
						OUTPUT[j] = OUTPUT[j] + V_EM * sign;
					}
				}
				else
				{
					if (OUTPUT.size() != 2) return 2;

					OUTPUT[0] = OUTPUT[0] + R_EM * sign;
					OUTPUT[1] = OUTPUT[1] + V_EM * sign;
				}
			}
		}
		else
		{
			MATRIX3 Rot;
			bool right;

			if (conv[i] == 2 || conv[i] == 3)
			{
				//Lunar libration matrix (MCI to MCT)
				if (PLEFEM(5, GMT / 3600.0, 0, NULL, NULL, NULL, &Rot))
				{
					return 1;
				}
			}
			else if (conv[i] == 4 || conv[i] == 5)
			{
				//Get closest RNP matrix to input time (ECI to ECT)
				//Time in days between first matrix and input time
				double dt = GetGMTBase() - EZNPMATX.mjd0 + GMT / 24.0 / 3600.0;
				int ii = (int)(floor(dt * 4.0 + 0.5));
				if (ii < 0 || ii > 140)
				{
					//Not available
					return 1;
				}
				Rot = EZNPMATX.Mat[ii];
			}
			else
			{
				VECTOR3 R_EM, V_EM, R_ES;
				VECTOR3 X_EMP, Y_EMP, Z_EMP;

				if (PLEFEM(1, GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL))
				{
					return 1;
				}

				X_EMP = -unit(R_EM);
				Z_EMP = unit(crossp(R_EM, V_EM));
				Y_EMP = crossp(Z_EMP, X_EMP);
				Rot = _M(X_EMP.x, X_EMP.y, X_EMP.z, Y_EMP.x, Y_EMP.y, Y_EMP.z, Z_EMP.x, Z_EMP.y, Z_EMP.z);
			}

			if (conv[i] == 2 || conv[i] == 4 || conv[i] == 6)
			{
				right = true;
			}
			else
			{
				right = false;
			}

			//MCTVR desired and conversion is MCT to MCI. Add rotational velocity.
			if (IsMCTVR && conv[i] == 3)
			{
				OUTPUT[1] += crossp(_V(0, 0, 1)*OrbMech::w_Moon, OUTPUT[0]);
			}

			if (right)
				{
					for (unsigned j = 0; j < OUTPUT.size(); j++)
					{
						OUTPUT[j] = mul(Rot, OUTPUT[j]);
					}
				}
			else
			{
				for (unsigned j = 0; j < OUTPUT.size(); j++)
				{
					OUTPUT[j] = tmul(Rot, OUTPUT[j]);
				}
			}

			//MCTVR desired and conversion is MCI to MCT. Subtract rotational velocity.
			if (IsMCTVR && conv[i] == 2)
			{
				OUTPUT[1] -= crossp(_V(0, 0, 1)*OrbMech::w_Moon, OUTPUT[0]);
			}
		}
	}

	return 0;
}

//Extended Interpolation Routine
void RTCC::ELVCTR(const ELVCTRInputTable &in, ELVCTROutputTable2 &out)
{
	EphemerisDataTable2 EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;
	unsigned vec_tot = in.ORER * 2;
	unsigned vec_bef = in.ORER;

	if (ELFECH(in.GMT, vec_tot, vec_bef, in.L, EPHEM, MANTIMES, LUNSTAY))
	{
		out.ErrorCode = 128;
		return;
	}

	ELVCTR(in, out, EPHEM, MANTIMES, &LUNSTAY);
}

void RTCC::ELVCTR(const ELVCTRInputTable &in, ELVCTROutputTable2 &out, EphemerisDataTable2 &EPH, ManeuverTimesTable &mantimes, LunarStayTimesTable *LUNRSTAY)
{
	//Is order of interpolation correct?
	if (in.ORER == 0 || in.ORER > 8)
	{
		out.ErrorCode = 64;
		return;
	}

	unsigned nvec = 0;
	double TS_stored = 0, TE_stored = 0;
	bool restore = false;
	unsigned ORER = in.ORER;
	int I;
	out.VPI = 0;
	out.ErrorCode = 0;

	if (in.GMT < EPH.table[0].GMT)
	{
		out.ErrorCode = 8;
		return;
	}
	if (in.GMT > EPH.table.back().GMT)
	{
		out.ErrorCode = 16;
		return;
	}
	out.TUP = EPH.Header.TUP;
	if (LUNRSTAY != NULL)
	{
		if (in.GMT >= LUNRSTAY->LunarStayBeginGMT && in.GMT <= LUNRSTAY->LunarStayEndGMT)
		{
			out.VPI = -1;
		}
	}
	I = 0;
	if (mantimes.Table.size() > 0)
	{
		double TS = EPH.Header.TL;
		double TE = EPH.Header.TR;
		unsigned J = 0;
		for (J = 0;J < mantimes.Table.size();J++)
		{
			//Equal to maneuver initiate time, go directly to 5A
			if (mantimes.Table[J].ManData[1] == in.GMT)
			{
				goto RTCC_ELVCTR_5A;
			}
			else if (mantimes.Table[J].ManData[1] > in.GMT)
			{
				//Equal to maneuver end time, go directly to 5A
				if (mantimes.Table[J].ManData[0] == in.GMT)
				{
					goto RTCC_ELVCTR_5A;
				}
				//Inside burn
				if (mantimes.Table[J].ManData[0] < in.GMT)
				{
					out.VPI = 1;
					goto RTCC_ELVCTR_3;
				}
				if (J == 0)
				{
					//Maneuver outside ephemeris range, let ELVARY handle the error
					if (mantimes.Table[J].ManData[0] > TE)
					{
						goto RTCC_ELVCTR_5A;
					}
					//Constrain ephemeris end to begin of first maneuver
					TE = mantimes.Table[J].ManData[0];
					goto RTCC_ELVCTR_H;
				}
				else
				{
					if (mantimes.Table[J].ManData[0] < TE)
					{
						TE = mantimes.Table[J].ManData[0];
					}
					break;
				}
			}
		}
		//Constrain ephemeris start to end of previous maneuver
		if (mantimes.Table[J - 1].ManData[1] > TS)
		{
			TS = mantimes.Table[J - 1].ManData[1];
		}
		goto RTCC_ELVCTR_H;
	RTCC_ELVCTR_3:
		ORER = 1;
		unsigned E = 0;
		while (EPH.table[E].GMT <= in.GMT)
		{
			//Direct hit
			if (EPH.table[E].GMT == in.GMT)
			{
				goto RTCC_ELVCTR_5A;
			}
			E++;
		}
		TE = EPH.table[E].GMT;
		TS = EPH.table[E - 1].GMT;
	RTCC_ELVCTR_H:
		unsigned V = 0;
		while (TS >= EPH.table[V].GMT)
		{
			if (TS == EPH.table[V].GMT)
			{
				goto RTCC_ELVCTR_4;
			}
			V++;
		}
		out.ErrorCode = 255;
		return;
	RTCC_ELVCTR_4:
		//Save stuff
		restore = true;
		nvec = EPH.Header.NumVec;
		TS_stored = EPH.Header.TL;
		TE_stored = EPH.Header.TR;
		unsigned NV = 1;
		while (abs (TE - EPH.table[NV - 1].GMT) > 0.000001) //To avoid floating point issues
		{
			if (TE < EPH.table[NV - 1].GMT)
			{
				out.ErrorCode = 255;
				return;
			}
			NV++;
		}
		EPH.Header.NumVec = NV - V;
		EPH.Header.Offset = V;
		EPH.Header.TL = TS;
		EPH.Header.TR = TE;
	}

RTCC_ELVCTR_5A:
	out.ErrorCode = ELVARY(EPH, ORER, in.GMT, false, out.SV, out.ORER);
	if (restore)
	{
		EPH.Header.NumVec = nvec;
		EPH.Header.Offset = 0;
		EPH.Header.TL = TS_stored;
		EPH.Header.TR = TE_stored;
	}
	return;
}

//Fixed point centiseconds to floating point hours
double RTCC::GLCSTH(double FIXCSC)
{
	return FIXCSC / 360000.0;
}

//Entry to GLCSTH
double RTCC::GLHTCS(double FLTHRS)
{
	return FLTHRS * 360000.0;
}

void RTCC::GLFDEN(double ALT, double &DENS, double &SPOS)
{
	double H, lnRho;
	int i;

	if (ALT < 0)
	{
		H = 0;
		//Set index to work at sea level
		i = 7;
		goto RTCC_GLFDEN_1;
	}
	if (ALT >= 700.0*1000.0)
	{
		DENS = 0.0;
		SPOS = SystemParameters.MHGDEN.CSMAX;
		return;
	}

	double T_M, D_apo, r;
	
	if (ALT > 90.0*1000.0)
	{
		r = SystemParameters.MHGDEN.r0 + ALT;
		SPOS = SystemParameters.MHGDEN.CSMAX;
	}
	else if (ALT == 90.0*1000.0)
	{
		//Set index to work at 90km
		i = 12;
		SPOS = SystemParameters.MHGDEN.CSMAX;
		goto RTCC_GLFDEN_3;
	}
	else
	{
		H = SystemParameters.MHGDEN.r0 * ALT / (SystemParameters.MHGDEN.r0 + ALT);
		goto RTCC_GLFDEN_4;
	}
	//2
	i = 0;
	while (r < SystemParameters.MHGDEN.r_b[i])
	{
		i++;
	}
	if (r == SystemParameters.MHGDEN.r_b[i])
	{
	RTCC_GLFDEN_3:
		D_apo = 0.0;
		T_M = SystemParameters.MHGDEN.T_M_b[i];
	}
	else
	{
		double F, G;
		T_M = SystemParameters.MHGDEN.T_M_b[i] + SystemParameters.MHGDEN.L_M[i] * (r - SystemParameters.MHGDEN.r_b[i]);
		F = T_M / SystemParameters.MHGDEN.T_M_b[i];
		G = SystemParameters.MHGDEN.r_b[i] / r;
		D_apo = SystemParameters.MHGDEN.B7[i] * (G - 1.0 + SystemParameters.MHGDEN.A7[i] * log(G*F));
	}
	double ln_rho_T_M, rho_T_M;
	ln_rho_T_M = SystemParameters.MHGDEN.ln_rho_b_T_M_b[i] - D_apo;
	rho_T_M = exp(ln_rho_T_M);
	DENS = rho_T_M / T_M;
	return;
RTCC_GLFDEN_4:
	i = 0;
	while (H < SystemParameters.MHGDEN.H_b[i])
	{
		i++;
	}
	if (H == SystemParameters.MHGDEN.H_b[i])
	{
	RTCC_GLFDEN_1:
		D_apo = 0.0;
		T_M = SystemParameters.MHGDEN.T_M_B[i];
		lnRho = SystemParameters.MHGDEN.ln_rho_b[i];
	}
	else
	{
		double D;

		D = SystemParameters.MHGDEN.B[i] * (H - SystemParameters.MHGDEN.H_b[i]);
		if (SystemParameters.MHGDEN.L_M_B[i] == 0.0)
		{
			D_apo = D;
			T_M = SystemParameters.MHGDEN.T_M_B[i];
			lnRho = SystemParameters.MHGDEN.ln_rho_b[i] - D_apo;
		}
		else
		{
			T_M = SystemParameters.MHGDEN.T_M_B[i] * (1.0 + D);
			D_apo = SystemParameters.MHGDEN.A[i] * log(1.0 + D);
			lnRho = SystemParameters.MHGDEN.ln_rho_b[i] - D_apo;
		}
	}
	SPOS = sqrt(SystemParameters.MHGDEN.C2*T_M);
	DENS = exp(lnRho);
}

VECTOR3 RTCC::GLMRTV(VECTOR3 A, double THET1, int K1, double THET2, int K2, double THET3, int K3)
{
	MATRIX3 Rot;
	VECTOR3 temp;
	double cos_thet, sin_thet;
	int Karr[3] = { K1,K2,K3 };
	double THETarr[3] = { THET1, THET2, THET3 };
	int i = 0;
	temp = A;
RTCC_GLMRTV_1:
	if (Karr[i] == 0)
	{
		return temp;
	}
	cos_thet = cos(THETarr[i]);
	sin_thet = sin(THETarr[i]);
	if (Karr[i] == 1)
	{
		Rot = _M(1, 0, 0, 0, cos_thet, sin_thet, 0, -sin_thet, cos_thet);
	}
	else if (Karr[i] == 2)
	{
		Rot = _M(cos_thet, 0, -sin_thet, 0, 1, 0, sin_thet, 0, cos_thet);
	}
	else
	{
		Rot = _M(cos_thet, sin_thet, 0, -sin_thet, cos_thet, 0, 0, 0, 1);
	}
	temp = mul(Rot, temp);
	i++;
	if (i < 3)
	{
		goto RTCC_GLMRTV_1;
	}
	return temp;
}

MATRIX3 RTCC::GLMRTM(MATRIX3 A, double THET1, int K1, double THET2, int K2, double THET3, int K3)
{
	//A = matrix to rotate
	//THET1 = angle of 1st rotation
	//K1 = axis of first rotation, 1 = X, 2 = Y, 3 = Z
	MATRIX3 Rot, temp;
	double cos_thet, sin_thet;
	int Karr[3] = { K1,K2,K3 };
	double THETarr[3] = { THET1, THET2, THET3 };
	int i = 0;
	temp = A;
RTCC_GLMRTM_1:
	if (Karr[i] == 0)
	{
		return temp;
	}
	cos_thet = cos(THETarr[i]);
	sin_thet = sin(THETarr[i]);
	if (Karr[i] == 1)
	{
		Rot = _M(1, 0, 0, 0, cos_thet, sin_thet, 0, -sin_thet, cos_thet);
	}
	else if (Karr[i] == 2)
	{
		Rot = _M(cos_thet, 0, -sin_thet, 0, 1, 0, sin_thet, 0, cos_thet);
	}
	else
	{
		Rot = _M(cos_thet, sin_thet, 0, -sin_thet, cos_thet, 0, 0, 0, 1);
	}
	temp = mul(Rot, temp);
	i++;
	if (i < 3)
	{
		goto RTCC_GLMRTM_1;
	}
	return temp;
}

double RTCC::GLQATN(double S, double C) const
{
	double T = atan2(S, C);
	if (T < 0)
	{
		return T + PI2;
	}
	return T;
}

//Subsatellite position
int RTCC::GLSSAT(VECTOR3 R, double GMT, int RBI, double &lat, double &lng, double &alt)
{
	VECTOR3 R_out, u;
	double K;
	int in, out;
	if (RBI == BODY_EARTH)
	{
		in = 0;
		out = 1;
		K = 1.0;
	}
	else
	{
		in = 2;
		out = 3;
		K = 0.0;
	}

	if (ELVCNV(R, GMT, 1, in, out, R_out))
	{
		return 1;
	}
	u = unit(R_out);
	lat = atan2(u.z, sqrt(u.x*u.x + u.y*u.y));
	lng = atan2(u.y, u.x) - OrbMech::w_Earth*K*GMT;
	OrbMech::normalizeAngle(lng, false);
	if (RBI == BODY_EARTH)
	{
		alt = length(R) - OrbMech::R_Earth;
	}
	else
	{
		alt = length(R) - BZLAND.rad[RTCC_LMPOS_BEST];
	}
	return 0;
}

bool EVTRANS(double alpha, double &F1, double &F2, double &F3, double &F4)
{
	F1 = OrbMech::stumpS(alpha);
	F2 = OrbMech::stumpC(alpha);
	F3 = 1.0 - alpha * F1;
	F4 = 1.0 - alpha * F2;
	return false;
}

void RTCC::GLUNIV(const GLUNIVInput &in, GLUNIVOutput &out)
{
	VECTOR3 H;
	double mu, r0, v0sq, T, d0, D0, s0, c0, ee, e, E0, DT, I_Per, A0, beta, alpha, Dbeta0, F1, F2, F3, F4, r, sqmu_dt, dbeta, beta0;
	double f, g, fdot, gdot, hh, h, p, tan_theta05, cot_DE05, DE, dE, R_e, c, cc, E, sing, cosg;
	int count, iter;

	const double eps0 = 2e-7;
	const double eps1 = 2e-6*OrbMech::R_Earth;

	mu = in.SQRMU*in.SQRMU;
	r0 = length(in.R0);
	v0sq = dotp(in.V0, in.V0);
	T = v0sq / mu - 2.0 / r0;
	d0 = dotp(in.R0, in.V0);
	D0 = d0 / in.SQRMU;

	if (T == 0.0)
	{
		//Parabolic orbit
		out.ERR = 1;
		return;
	}
	if (in.Ind == 0)
	{
		goto RTCC_GLUNIV_2;
	}
	if (in.Ind == 3)
	{
		goto RTCC_GLUNIV_5;
	}
	c0 = 1.0 + r0 * T;
	ee = c0 * c0 - D0 * D0*T;
	e = sqrt(ee);
	s0 = D0 * sqrt(abs(T));
	if (T > 0)
	{
		E0 = log((s0 + c0) / e);
	}
	else
	{
		E0 = atan2(s0, c0);
		if (E0 < 0)
		{
			E0 += PI2;
		}
	}
	if (in.Ind == 1)
	{
		goto RTCC_GLUNIV_9;
	}
	if (in.Ind == 2)
	{
		goto RTCC_GLUNIV_11;
	}
	if (in.Ind == 4)
	{
		goto RTCC_GLUNIV_12;
	}
	out.ERR = 2;
	return;
RTCC_GLUNIV_2:
	DT = in.PARM - in.T0;
	if (T > 0)
	{
		I_Per = 0.0;
	}
	else
	{
		double Per, I;
		Per = PI2 / sqrt(pow(-T, 3)) / in.SQRMU;
		I = floor(abs(DT / Per));
		if (I > 0 && DT < 0)
		{
			I = -I;
		}
		I_Per = I * Per;
		DT = DT - I_Per;
	}
	A0 = in.SQRMU*DT;
	beta = in.SQRMU*DT / (5.0*r0);
	if (T > 0)
	{
		if (abs(beta) >= PI * 4.0 / sqrt(T))
		{
			beta = abs(beta / beta * PI*4.0 / sqrt(T));
		}
	}
	Dbeta0 = 0.0;
	count = 20;
RTCC_GLUNIV_LOPP:
	alpha = -T * beta*beta;
	if (EVTRANS(alpha, F1, F2, F3, F4))
	{
		out.ERR = 9;
		return;
	}
	r = beta * beta*F2 + r0 * F4 + D0 * beta*F3;
	sqmu_dt = beta * (beta*beta*F1 + r0 * F3 + D0 * beta*F2);
	dbeta = (A0 - sqmu_dt) / r;
	if (abs(dbeta) > eps0)
	{
		count = count - 1;
		if (count <= 0)
		{
			out.ERR = 3;
			return;
		}
		if (dbeta*Dbeta0 >= 0 || abs(dbeta) <= abs(-0.5*Dbeta0))
		{
			Dbeta0 = dbeta;
			beta = beta + dbeta;
			beta0 = beta;
		}
		else
		{
			beta = beta0 - 0.5*Dbeta0;
			beta0 = beta;
			Dbeta0 = 0.0;
		}
		goto RTCC_GLUNIV_LOPP;
	}
	DT = DT + I_Per;
RTCC_GLUNIV_3:
	f = 1.0 - beta * beta*F2 / r0;
	gdot = 1.0 - beta * beta*F2 / r;
	g = beta / in.SQRMU*(r0*F3 + D0 * beta*F2);
	fdot = -in.SQRMU / r / r0 * beta*F3;
RTCC_GLUNIV_4_1:
	out.T1 = in.T0 + DT;
	out.R1 = in.R0*f + in.V0*g;
	out.V1 = in.R0*fdot + in.V0*gdot;
	if (in.Ind != 1)
	{
		out.ERR = 0;
		return;
	}
	out.R_NEW = OrbMech::R_Earth; //out.R_NEW = r/sqrt(out.R1.x*out.R1.x+out.R1.y*out.R1.y+in.PARM3*out.R1.z*out.R1.z);
	if (abs(R_e - out.R_NEW) <= eps1)
	{
		out.ERR = 0;
		return;
	}
	iter = iter - 1;
	if (iter <= 0)
	{
		out.ERR = 5;
		return;
	}
	R_e = out.R_NEW;
	goto RTCC_GLUNIV_10;
RTCC_GLUNIV_5:
	if (abs(in.PARM) < 1e-10)
	{
		DT = 0.0;
		f = 1.0;
		gdot = 1.0;
		g = 0.0;
		fdot = 0.0;
		goto RTCC_GLUNIV_4_1;
	}
	if (abs(in.PARM) > PI2)
	{
		out.ERR = 2;
		return;
	}
	H = crossp(in.R0, in.V0);
	hh = dotp(H, H);
	p = hh / mu;
	h = sqrt(hh);
	if (abs(abs(in.PARM / 2.0) - PI05) < 1e-10)
	{
		tan_theta05 = 1e10;
	}
	else
	{
		tan_theta05 = tan(in.PARM);
	}
	cot_DE05 = sqrt(abs(p / T)) / r0 * (1.0 / tan_theta05 - d0 / h);
	if (T > 0)
	{
		//Hyperbolic
		double L = (cot_DE05 + 1.0) / (cot_DE05 - 1.0);
		if (L < 0)
		{
			out.ERR = 8;
			return;
		}
		DE = log(L);
		if (DE <= 0 && in.PARM > 0)
		{
			out.ERR = 2;
			return;
		}
	}
	else
	{
		if (abs(in.PARM) > PI2 - 1e-10)
		{
			DE = PI2;
		}
		else
		{
			if (in.PARM >= 0)
			{
				dE = 0.0;
			}
			else
			{
				dE = PI2;
			}
			double cosDE = (cot_DE05 - 1.0) / (cot_DE05 + 1.0);
			double sinDE = (1.0 + cosDE) / (cot_DE05);
			DE = atan2(sinDE, cosDE);
			DE = DE - dE;
		}
	}
RTCC_GLUNIV_6:
	beta = DE / sqrt(abs(T));
	alpha = -T * beta*beta;
	if (EVTRANS(alpha, F1, F2, F3, F4))
	{
		out.ERR = 9;
		return;
	}
	r = beta * beta*F2 + r0 * F4 + D0 * beta*F3;
	DT = beta / in.SQRMU*(beta*beta*F1 + r0 * F3 + D0 * beta*F2);
	goto RTCC_GLUNIV_3;
RTCC_GLUNIV_9:
	iter = 10;
	R_e = in.PARM;
RTCC_GLUNIV_10:
	r = R_e + in.PARM2;
	c = 1.0*r*T;
	cc = c * c;
	if (T > 0)
	{
		if (cc < ee)
		{
			out.ERR = 4;
			return;
		}
		E = -log((c + sqrt(cc - ee)) / e);
	}
	else
	{
		if (cc > ee)
		{
			out.ERR = 4;
			return;
		}
		E = -atan(sqrt(ee - cc) / c);
	}
	DE = E - E0;
	if (DE < 0 && d0 >= 0)
	{
		if (T > 0)
		{
			out.ERR = 4;
			return;
		}
		DE = DE + PI2;
	}
	goto RTCC_GLUNIV_6;
RTCC_GLUNIV_11:
	c = 1.0 + in.PARM*T;
	cc = c * c;
	if (T > 0)
	{
		if (cc < ee)
		{
			out.ERR = 6;
			return;
		}
		E = log((c + sqrt(cc - ee)) / e);
	}
	else
	{
		if (cc > ee)
		{
			out.ERR = 6;
			return;
		}
		E = atan(sqrt(ee - cc) / c);
	}
	E = in.PARM3*E;
	DE = E - E0;
	if (DE > 0)
	{
		if (in.PARM3 < 0)
		{
			if (T > 0)
			{
				out.ERR = 6;
				return;
			}
			DE = DE - PI2;
		}
	}
	else
	{
		if (in.PARM3 > 0.0)
		{
			if (T > 0)
			{
				out.ERR = 6;
				return;
			}
			DE = DE + PI2;
		}
	}
	goto RTCC_GLUNIV_6;
RTCC_GLUNIV_12:
	//Flight Path Angle
	sing = sin(in.PARM);
	cosg = sqrt(1.0 - sing * sing);
	if (ee - sing * sing < 0)
	{
		//gamma impossible to attain
		out.ERR = 7;
		return;
	}
	if (T > 0)
	{
		double L = (sqrt(ee - 1.0)*sing + sqrt(ee - sing * sing)) / (e*cosg);
		if (L < 0)
		{
			out.ERR = 8;
			return;
		}
		E = log(L);
	}
	else
	{
		E = atan2(sqrt(1.0 - ee)*sing, sqrt(ee - sing * sing));
	}
	DE = E - E0;
	if (DE > 0)
	{
		if (in.PARM3 < 0)
		{
			if (T > 0)
			{
				out.ERR = 7;
				return;
			}
			DE = DE - PI2;
		}
	}
	else
	{
		if (in.PARM3 > 0.0)
		{
			if (T > 0)
			{
				out.ERR = 7;
				return;
			}
			DE = DE + PI2;
		}
	}
	goto RTCC_GLUNIV_6;
}

//Orbit Determination Subroutine
int RTCC::LLBRTD(EphemerisData sv, int I, double SQMU, double PARM, EphemerisData &sv_out)
{
	double R_E;
	return LLBRTD(sv, I, SQMU, PARM, 0.0, 0.0, sv_out, R_E);
}
int RTCC::LLBRTD(EphemerisData sv, int I, double SQMU, double PARM, double H, double SQRAT, EphemerisData &sv_out, double &R_E)
{
	//Input:
	//sv: Input state vector. R0, V0, GMT0
	//I: Indicator. 0 for time, < 0 for altitude, > 0 for anomaly
	//SQMU: Square root of gravitational constant
	//PARM: Time for output if time option. Assumption for radius if altitude option. Change in true anomaly for anomaly option
	//H: Altitude above surface for output vector (altitude option)
	//SQRAT: Square of ratio of equatorial to polar radii
	//Output:
	//sv_out: Output state vector
	//return value: error indicator. 0 if no error, 4 if hyperbolic, 8 if no height intersection, 16 if no convergence after 5 iterations

	double R0, V02, sigma0, A, SQA, N, D, esinE, tanDE, cosDE, sinDE, DE, dE, S0, S1, S2, S3, f, g, fdot, gdot, dt_c, R_E_old, h, sinDV, cosDV, DT, R, esinE0, ecosE0, ecosE, esinE2;

	const double eps_R = 2.0e-6*OrbMech::R_Earth;
	const double eps_K = 2.0e-7;
	double mu = SQMU * SQMU;
	int iter = 0;

	sv_out.RBI = sv.RBI;

	//Compute R0, V0, sigma0, A
	R0 = length(sv.R);
	V02 = dotp(sv.V, sv.V);
	sigma0 = dotp(sv.R, sv.V);
	A = R0 / (2.0 - R0 * V02 / mu);
	if (A < 0)
	{
		//Error
		return 4;
	}
	SQA = sqrt(A);
	dE = 0.0;
	if (I > 0)
	{
		//Anomaly
		goto RTCC_LLBRTD_3;
	}
	if (I == 0)
	{
		//Time
		goto RTCC_LLBRTD_4;
	} 
	R_E = PARM;
RTCC_LLBRTD_1_1:
	//Compute r, esinE0, ecosE0, ecosE, esinE2
	R = R_E + H;
	esinE0 = sigma0 / SQMU / SQA;
	ecosE0 = 1.0 - R0 / A;
	ecosE = 1.0 - R / A;
	esinE2 = esinE0 * esinE0 + ecosE0 * ecosE0 - ecosE * ecosE;
	if (esinE2 < 0)
	{
		//Error
		return 8;
	}
	if (sigma0 < 0) //Negative flight path angle
	{
		dE = 0.0;
		if (R > R0)
		{
			dE = PI2;
		}
	}
	//Compute esinE, N, D
	esinE = -sqrt(esinE2); //minus ensures reentry
	N = esinE * ecosE0 - ecosE * esinE0;
	D = ecosE * ecosE0 + esinE * esinE0;
RTCC_LLBRTD_2_1:
	//Compute tanDE, cosDE, sinDE, DE
	tanDE = N / D;
	if (D > 0)
	{
		cosDE = sqrt(1.0 + tanDE * tanDE);
	}
	else
	{
		cosDE = -sqrt(1.0 + tanDE * tanDE);
	}
	cosDE = 1.0 / cosDE; //The above is actually the secant
	sinDE = tanDE * cosDE;
	DE = atan2(sinDE, cosDE); //Was N/D in the IBM RTCC document
	if (DE < 0)
	{
		DE += PI2;
	}
	DE = DE + dE;
	//Compute S0, S1, S2, S3, R, g, dt_c, T
	S0 = cosDE;
	S1 = SQA / SQMU * sinDE;
	S2 = A / mu *(1.0 - cosDE);
	S3 = pow(SQA / SQMU, 3)*(DE - sinDE);
	R = R0 * S0 + sigma0 * S1 + S2 * mu;
	g = R0 * S1 + sigma0 * S2;
	dt_c = g + S3 * mu;
	sv_out.GMT = sv.GMT + dt_c;
RTCC_LLBRTD_2_2:
	//Compute f, fdot, gdot, RV, VV
	f = 1.0 - mu *S2 / R0;
	fdot = -S1 * mu / (R*R0);
	gdot = 1.0 - S2 * mu / R;
	sv_out.R = sv.R*f + sv.V*g;
	sv_out.V = sv.R*fdot + sv.V*gdot;
	if (I >= 0) return 0;
	R_E_old = R_E;
	R_E = OrbMech::R_Earth;//R / sqrt(sv_out.R.x*sv_out.R.x + sv_out.R.y*sv_out.R.y + sv_out.R.z*sv_out.R.z / SQRAT); //Change when units are Earth radii
	if (abs(R_E - R_E_old) <= eps_R)
	{
		return 0;
	}
	goto RTCC_LLBRTD_1_1;
RTCC_LLBRTD_3:
	if (PARM < 0)
	{
		dE = PI2;
	}
	//Compute h, sinDV, cosDV, N, D
	h = sqrt(R0*R0*V02 - sigma0 * sigma0);
	sinDV = sin(PARM);
	cosDV = cos(PARM);
	N = R0 / SQA / SQMU * (h*sinDV - sigma0 * (1.0 - cosDV));
	D = h * h / mu - (h*h / mu - R0 + R0 * R0 / A)*(1.0 - cosDV) - sigma0 * h / mu * sinDV;
	goto RTCC_LLBRTD_2_1;
RTCC_LLBRTD_4:
	//Compute DT, DE
	DT = PARM - sv.GMT;
	DE = DT * SQMU / pow(SQA, 3);
RTCC_LLBRTD_4_1:
	//Compute sinDE, cosDE
	sinDE = sin(DE);
	cosDE = cos(DE);
	//Compute S0, S1, S2, S3, R, g, dt_c, T
	S0 = cosDE;
	S1 = SQA / SQMU * sinDE;
	S2 = A / mu * (1.0 - cosDE);
	S3 = pow(SQA / SQMU, 3)*(DE - sinDE);
	R = R0 * S0 + sigma0 * S1 + S2 * mu;
	g = R0 * S1 + sigma0 * S2;
	dt_c = g + S3 * mu;
	sv_out.GMT = sv.GMT + dt_c;
	//Compute dE
	dE = (DT - dt_c)*SQMU / R / SQA;
	if (abs(dE) <= eps_K)
	{
		goto RTCC_LLBRTD_2_2;
	}
	if (iter < 5)
	{
		DE = DE + dE;
		iter++;
		goto RTCC_LLBRTD_4_1;
	}
	//Error
	return 16;
}

//Weight Access Routine
void RTCC::PLAWDT(const PLAWDTInput &in, PLAWDTOutput &out)
{
	//Time of area and weights in GET
	double T_AW;
	//Update time for weights, can be changed internally, so make it an internal variable, in GMT
	double T_UP;
	//Configuration code
	std::bitset<4> CC;
	//Time to stop venting
	double T_NV;
	double dt;
	unsigned int J, N, K, BLK;

	MissionPlanTable *mpt;

	//No error yet...
	out.Err = 0;
	dt = 0.0;
	T_UP = in.T_UP;

	if (in.TableCode < 0)
	{
		//Option 2
		CC = in.Num;
		T_AW = in.T_IN - GetGMTLO()*3600.0;
		//Input time is greater than output time
		if (in.T_IN > T_UP)
		{
			goto RTCC_PLAWDT_9_T;
		}
		if (in.VentingOpt == false)
		{
			goto RTCC_PLAWDT_3_G;
		}
		if (CC[RTCC_CONFIG_S] == false)
		{
			goto RTCC_PLAWDT_3_G;
		}
		//We need to consider S-IVB venting
		mpt = GetMPTPointer(-in.TableCode);
		//Search for TLI
		bool tli = false;
		unsigned tlinum;
		for (unsigned i = 0;i < mpt->ManeuverNum;i++)
		{
			if (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
			{
				tli = true;
				tlinum = i;
			}
		}
		//Do we have a TLI?
		if (tli)
		{
			//Yes, end time for venting is TLI plus MCGVNT
			T_NV = mpt->TimeToEndManeuver[tlinum] + SystemParameters.MCGVNT*3600.0;
		}
		else
		{
			//No, end time for venting is requested time
			T_NV = T_UP;
		}
		//Convert to GET
		T_NV = T_NV - GetGMTLO()*3600.0;
		goto RTCC_PLAWDT_3_G;
	}
	//Option 1
	mpt = GetMPTPointer(in.TableCode);
	if (in.KFactorOpt)
	{
		out.KFactor = mpt->KFactor;
	}
	//Search for TLI
	bool tli = false;
	unsigned tlinum;
	for (unsigned i = 0;i < mpt->ManeuverNum;i++)
	{
		if (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
		{
			tli = true;
			tlinum = i;
		}
	}
	//Do we have a TLI?
	if (tli)
	{
		//Yes, end time for venting is TLI plus MCGVNT
		T_NV = mpt->TimeToEndManeuver[tlinum] + SystemParameters.MCGVNT*3600.0;
	}
	else
	{
		//No, end time for venting is requested time
		T_NV = T_UP;
	}
	//Convert to GET
	T_NV = T_NV - GetGMTLO()*3600.0;
	//How many maneuvers on MPT?
	if (mpt->ManeuverNum == 0)
	{
		K = 0;
		CC = 0;
		T_AW = mpt->SIVBVentingBeginGET;
		goto RTCC_PLAWDT_3_F;
	}
	else
	{
		N = mpt->ManeuverNum;
		CC = 0;
		K = in.Num;
		//K can't be greater than N
		if (K > N)
		{
			K = N;
		}
		J = N - K;
	}
	if (J == 0)
	{
		goto RTCC_PLAWDT_2_B;
	}
RTCC_PLAWDT_2_C:
	if (T_UP < mpt->TimeToBeginManeuver[K])
	{
		K = N - J;
		goto RTCC_PLAWDT_2_B;
	}
	else if (T_UP == mpt->TimeToBeginManeuver[K])
	{
		K = N - J;
		CC = mpt->mantable[N - J].ConfigCodeBefore;
		goto RTCC_PLAWDT_2_B;
	}
	K++;
	if (J != 1)
	{
		J = J - 1;
		goto RTCC_PLAWDT_2_C;
	}
	if (T_UP < mpt->TimeToEndManeuver[K - 1])
	{
		out.Err = 1;
		//K = N - (J + 1);
		K = N - J;
		goto RTCC_PLAWDT_2_B;
	}
RTCC_PLAWDT_2_B:
	BLK = K;
RTCC_PLAWDT_2_D:
	if (K == 0)
	{
		T_AW = mpt->SIVBVentingBeginGET;
		goto RTCC_PLAWDT_3_F;
	}
	//Maneuver not current?
	if (K > mpt->LastExecutedManeuver && (abs(mpt->CommonBlock.TUP) != abs(mpt->mantable[K - 1].CommonBlock.TUP)))
	{
		out.Err = 2;
		K = K - 1;
		goto RTCC_PLAWDT_2_D;
	}
	T_AW = mpt->TimeToEndManeuver[K - 1];
RTCC_PLAWDT_3_F:
	if (CC == 0)
	{
		if (K == 0)
		{
			CC = mpt->CommonBlock.ConfigCode;
		}
		else
		{
			CC = mpt->mantable[K - 1].CommonBlock.ConfigCode;
		}
	}
RTCC_PLAWDT_3_G:
	//Move areas and weights to output
	if (in.TableCode > 0)
	{
		MPTVehicleDataBlock *tempb;
		if (K == 0)
		{
			tempb = &mpt->CommonBlock;
		}
		else
		{
			tempb = &mpt->mantable[K - 1].CommonBlock;
		}
		out.CSMWeight = tempb->CSMMass;
		out.CSMArea = tempb->CSMArea;
		out.SIVBWeight = tempb->SIVBMass;
		out.SIVBArea = tempb->SIVBArea;
		out.LMAscWeight = tempb->LMAscentMass;
		out.LMAscArea = tempb->LMAscentArea;
		out.LMDscWeight = tempb->LMDescentMass;
		out.LMDscArea = tempb->LMDescentArea;
		out.CC = CC;
	}
	else
	{
		out.CSMWeight = in.CSMWeight;
		out.CSMArea = in.CSMArea;
		out.SIVBWeight = in.SIVBWeight;
		out.SIVBArea = in.SIVBArea;
		out.LMAscWeight = in.LMAscWeight;
		out.LMAscArea = in.LMAscArea;
		out.LMDscWeight = in.LMDscWeight;
		out.LMDscArea = in.LMDscArea;
		out.CC = in.Num;
	}
	//Read Expendables Table
	//Make T_UP a GET
	T_UP = T_UP - GetGMTLO()*3600.0;
	//TBD: Expendables stuff
	dt = T_UP - T_AW;
RTCC_PLAWDT_M_5:
	if (CC[RTCC_CONFIG_C])
	{
		out.CSMWeight = out.CSMWeight - (SystemParameters.MDVCCC[0]*dt+ SystemParameters.MDVCCC[1]);
	}
	if (CC[RTCC_CONFIG_A])
	{
		out.LMAscWeight = out.LMAscWeight - (SystemParameters.MDVACC[0] * dt + SystemParameters.MDVACC[1]);
	}
	if (CC[RTCC_CONFIG_D])
	{
		out.LMDscWeight = out.LMDscWeight - (SystemParameters.MDVDCC[0] * dt + SystemParameters.MDVDCC[1]);
	}
	out.ConfigWeight = 0.0;
	out.ConfigArea = 0.0;
	if (CC[RTCC_CONFIG_C])
	{
		out.ConfigWeight = out.ConfigWeight + out.CSMWeight;
		out.ConfigArea = max(out.ConfigArea, out.CSMArea);
	}
	if (CC[RTCC_CONFIG_S] == false)
	{
		goto RTCC_PLAWDT_8_Z;
	}
	if (in.VentingOpt == false)
	{
		goto RTCC_PLAWDT_8_Y;
	}
	if (out.Err == 3)
	{
		goto RTCC_PLAWDT_8_Y;
	}
	if (T_AW < SystemParameters.MCGVEN)
	{
		T_AW = SystemParameters.MCGVEN;
	}
	if (T_UP < SystemParameters.MCGVEN)
	{
		goto RTCC_PLAWDT_8_Y;
	}
	if (T_AW == T_UP)
	{
		goto RTCC_PLAWDT_8_Y;
	}
	//Venting calculations
	if (T_UP > T_NV)
	{
		T_UP = T_NV;
	}
	double TV, Th;
	dt = 3.0*60.0;
	TV = T_AW + dt;
	N = 1;
	K = 10;
RTCC_PLAWDT_7_Q:
	if (TV > T_UP)
	{
		dt = T_UP - (TV - dt);
		TV = T_UP;
	}
RTCC_PLAWDT_7_R:
	if (TV == SystemParameters.MDTVTV[1][N-1])
	{
		Th = SystemParameters.MDTVTV[0][N - 1];
		goto RTCC_PLAWDT_8_WDOT;
	}
	else if (TV < SystemParameters.MDTVTV[1][N - 1])
	{
		Th = SystemParameters.MDTVTV[0][N - 2] + (SystemParameters.MDTVTV[0][N - 1] - SystemParameters.MDTVTV[0][N - 2]) / (SystemParameters.MDTVTV[1][N - 1] - SystemParameters.MDTVTV[1][N - 2])*(TV - SystemParameters.MDTVTV[1][N - 2]);
		goto RTCC_PLAWDT_8_WDOT;
	}
	N++;
	K--;
	if (K == 0)
	{
		dt = SystemParameters.MDTVTV[1][N - 1] - TV;
		Th = SystemParameters.MDTVTV[0][N - 1];
		goto RTCC_PLAWDT_8_WDOT;
	}
	goto RTCC_PLAWDT_7_R;
RTCC_PLAWDT_8_WDOT:
	out.SIVBWeight = out.SIVBWeight - dt * Th / SystemParameters.MCTVSP*SystemParameters.MCTVEN;
	if (TV < T_UP)
	{
		TV = TV + dt;
		goto RTCC_PLAWDT_7_Q;
	}
RTCC_PLAWDT_8_Y:
	out.ConfigWeight = out.ConfigWeight + out.SIVBWeight;
	out.ConfigArea = max(out.ConfigArea, out.SIVBArea);
RTCC_PLAWDT_8_Z:
	if (CC[RTCC_CONFIG_A])
	{
		out.ConfigWeight = out.ConfigWeight + out.LMAscWeight;
		out.ConfigArea = max(out.ConfigArea, out.LMAscArea);
	}
	else
	{
		out.LMAscArea = 0.0;
		out.LMAscWeight = 0.0;
	}
	if (CC[RTCC_CONFIG_D])
	{
		out.ConfigWeight = out.ConfigWeight + out.LMDscWeight;
		out.ConfigArea = max(out.ConfigArea, out.LMDscArea);
	}
	else
	{
		out.LMDscArea = 0.0;
		out.LMDscWeight = 0.0;
	}
	return;
RTCC_PLAWDT_9_T:
	out.Err = 3;
	//Move areas and weights to output
	out.CSMArea = in.CSMArea;
	out.SIVBArea = in.SIVBArea;
	out.LMAscArea = in.LMAscArea;
	out.LMDscArea = in.LMDscArea;
	out.CSMWeight = in.CSMWeight;
	out.SIVBWeight = in.SIVBWeight;
	out.LMAscWeight = in.LMAscWeight;
	out.LMDscWeight = in.LMDscWeight;
	goto RTCC_PLAWDT_M_5;
}

//Sun/Moon Ephemeris Interpolation Program
bool RTCC::PLEFEM(int IND, double HOUR, int YEAR, VECTOR3 *R_EM, VECTOR3 *V_EM, VECTOR3 *R_ES, MATRIX3 *PNL)
{
	//INPUTS:
	//IND: 1 = Sun and Moon ephemerides. 2 = all data. 3 = all Moon data. 4 = Moon ephemerides. 5 = libration matrix only
	double T, C[6];
	int i, j, k;

	if (IND > 0)
	{
		HOUR = HOUR + SystemParameters.MCCBES;
	}
	//TBD: Convert from universal time to ephemeris time
	//T is 0 to 1, from last to next 12 hour interval
	T = (HOUR - floor(HOUR / 12.0) * 12.0) / 12.0;
	C[0] = -((T + 1.0)*T*(T - 1.0)*(T - 2.0)*(T - 3.0)) / 120.0;
	C[1] = ((T + 2.0)*T*(T - 1.0)*(T - 2.0)*(T - 3.0)) / 24.0;
	C[2] = -((T + 2.0)*(T + 1.0)*(T - 1.0)*(T - 2.0)*(T - 3.0)) / 12.0;
	C[3] = ((T + 2.0)*(T + 1.0)*T*(T - 2.0)*(T - 3.0)) / 12.0;
	C[4] = -((T + 2.0)*(T + 1.0)*T*(T - 1.0)*(T - 3.0)) / 24.0;
	C[5] = ((T + 2.0)*(T + 1.0)*T*(T - 1.0)*(T - 2.0)) / 120.0;

	//Calculate MJD from GMT
	double MJD = SystemParameters.GMTBASE + HOUR / 24.0;
	//Calculate position of time in array
	i = (int)((MJD - MDGSUN.MJD)*2.0);
	//Calculate starting point in the array
	j = i - 2;
	//Is time contained in Sun/Moon data array?
	if (j < 0 || j > 65) goto RTCC_PLEFEM_A;

	double x[18];
	bool des[4] = {false, false, false, false};

	//Which data to get?
	switch (abs(IND))
	{
	case 1:
		des[0] = des[1] = des[2] = true;
		break;
	case 2:
		des[0] = des[1] = des[2] = des[3] = true;
		break;
	case 3:
		des[1] = des[2] = des[3] = true;
		break;
	case 4:
		des[1] = des[2] = true;
		break;
	case 5:
		des[3] = true;
		break;
	default:
		goto RTCC_PLEFEM_A;
	}

	for (k = 0;k < 18;k++)
	{
		if (k < 3)
		{
			//Sun position
			if (des[0] == false) continue;
		}
		else if (k < 6)
		{
			//Moon position
			if (des[1] == false) continue;
		}
		else if (k < 9)
		{
			//Moon velocity
			if (des[2] == false) continue;
		}
		else
		{
			//Moon libration
			if (des[3] == false) break;
		}
		x[k] = C[0] * MDGSUN.data[j][k] + C[1] * MDGSUN.data[j + 1][k] + C[2] * MDGSUN.data[j + 2][k] + C[3] * MDGSUN.data[j + 3][k] + C[4] * MDGSUN.data[j + 4][k] + C[5] * MDGSUN.data[j + 5][k];
	}
	if (des[0] && R_ES)
	{
		*R_ES = _V(x[0], x[1], x[2])*SystemParameters.MCCMCU;
	}
	if (des[1] && R_EM)
	{
		*R_EM = _V(x[3], x[4], x[5])*SystemParameters.MCCMCU;
	}
	if (des[2] && V_EM)
	{
		*V_EM = _V(x[6], x[7], x[8])*SystemParameters.MCCMCU / 3600.0;
	}
	if (des[3] && PNL)
	{
		*PNL = _M(x[9], x[10], x[11], x[12], x[13], x[14], x[15], x[16], x[17]);
	}
	
	return false;
RTCC_PLEFEM_A:
	//Error return
	return true;
}

//Coefficients of lift and drag interpolation subroutine
void RTCC::RLMCLD(double FMACH, int VEH, double &CD, double &CL)
{
	double ALFA;
	RLMCLD(FMACH, VEH, CD, CL, ALFA);
}
void RTCC::RLMCLD(double FMACH, int VEH, double &CD, double &CL, double &ALFA)
{
	//FMACH: Mach number for coefficients of lift and drag
	//VEH: Vehicle code (0 for CSM, 1 for LEM)
	double *tab;

	if (VEH == 0)
	{
		tab = SystemParameters.MHACLD;
	}
	else
	{
		tab = SystemParameters.MHALLD;
	}
	int i = 1;

	double *CDT = &tab[25];
	double *CLT = &tab[50];
	double *AOAT = &tab[75];

	while (FMACH < tab[i - 1] && i < 25)
	{
		i++;
	}
	if (i == 1 || FMACH == tab[i - 1])
	{
		CD = CDT[i - 1];
		CL = CLT[i - 1];
		ALFA = AOAT[i - 1];
		return;
	}

	CD = CDT[i - 1] + (FMACH - tab[i - 1]) / (tab[i - 1] - tab[i - 2])*(CDT[i - 1] - CDT[i - 2]);
	CL = CLT[i - 1] + (FMACH - tab[i - 1]) / (tab[i - 1] - tab[i - 2])*(CLT[i - 1] - CLT[i - 2]);
	ALFA = AOAT[i - 1] + (FMACH - tab[i - 1]) / (tab[i - 1] - tab[i - 2])*(AOAT[i - 1] - AOAT[i - 2]);
}

//Computes and outputs pitch, yaw, roll
void RTCC::RLMPYR(VECTOR3 X_P, VECTOR3 Y_P, VECTOR3 Z_P, VECTOR3 X_B, VECTOR3 Y_B, VECTOR3 Z_B, double &Pitch, double &Yaw, double &Roll)
{
	double dot;
	dot = dotp(X_B, Y_P);
	if (abs(dot) > 1.0 - 0.0017)
	{
		Roll = 0.0;
		Pitch = atan2(dotp(Z_B, X_P), dotp(Z_B, Z_P));
	}
	else
	{
		Roll = atan2(dotp(-Z_B, Y_P), dotp(Y_B, Z_P));
		Pitch = atan2(dotp(-X_B, Z_P), dotp(X_B, X_P));
	}
	Yaw = asin(dot);
	if (Roll < 0)
	{
		Roll += PI2;
	}
	if (Pitch < 0)
	{
		Pitch += PI2;
	}
	if (Yaw < 0)
	{
		Yaw += PI2;
	}
}

//Computes time of longitude crossing
double RTCC::RLMTLC(EphemerisDataTable2 &EPHEM, ManeuverTimesTable &MANTIMES, double long_des, double GMT_min, double &GMT_cross, EphemerisData2 &sv_cur, LunarStayTimesTable *LUNRSTAY)
{
	if (EPHEM.Header.TUP <= 0) return -1.0;
	if (EPHEM.table.size() == 0) return -1.0;
	if (EPHEM.Header.CSI != 1 && EPHEM.Header.CSI != 3) return -1.0;

	ELVCTRInputTable interin;
	ELVCTROutputTable2 interout;
	double T1, dlng1, T2, dlng2, Tx, dlngx, w_E;
	unsigned i = 0;

	if (GMT_min > EPHEM.Header.TL)
	{
		if (GMT_min > EPHEM.Header.TR)
		{
			return -1.0;
		}
		T1 = GMT_min;
	}
	else
	{
		T1 = EPHEM.Header.TL;
	}

	while (T1 > EPHEM.table[i + 1].GMT)
	{
		i++;
	}

	interin.GMT = T1;
	ELVCTR(interin, interout, EPHEM, MANTIMES, LUNRSTAY);

	if (interout.ErrorCode)
	{
		return -1.0;
	}

	if (EPHEM.Header.CSI == RTCC_COORDINATES_ECT)
	{
		w_E = OrbMech::w_Earth;
	}
	else
	{
		w_E = 0.0;
	}

	sv_cur = interout.SV;
	dlng1 = atan2(sv_cur.R.y, sv_cur.R.x) - w_E * sv_cur.GMT - long_des;
	OrbMech::normalizeAngle(dlng1, false);

	if (abs(dlng1) < 0.0001)
	{
		GMT_cross = T1;
		return 0;
	}

	if (i >= EPHEM.table.size() - 1)
	{
		return -1.0;
	}

	while (i < EPHEM.table.size() - 1)
	{
		i++;
		sv_cur = EPHEM.table[i];
		T2 = sv_cur.GMT;

		dlng2 = atan2(sv_cur.R.y, sv_cur.R.x) - w_E * sv_cur.GMT - long_des;
		OrbMech::normalizeAngle(dlng2, false);

		if (abs(dlng2) < 0.0001)
		{
			GMT_cross = T2;
			return 0;
		}

		if (dlng1*dlng2 < 0 && abs(dlng1 - dlng2) < PI)
		{
			break;
		}
		if (i == EPHEM.table.size() - 1)
		{
			return -1.0;
		}
		T1 = T2;
		dlng1 = dlng2;
	}

	int iter = 0;

	do
	{
		Tx = T1 - (T2 - T1) / (dlng2 - dlng1)*dlng1;

		interin.GMT = Tx;
		ELVCTR(interin, interout, EPHEM, MANTIMES, LUNRSTAY);

		if (interout.ErrorCode)
		{
			return -1.0;
		}

		sv_cur = interout.SV;

		dlngx = atan2(sv_cur.R.y, sv_cur.R.x) - w_E * sv_cur.GMT - long_des;
		OrbMech::normalizeAngle(dlngx, false);

		if (abs(dlngx) < 0.0001)
		{
			GMT_cross = Tx;
			return 0;
		}
		if (abs(dlng2) > abs(dlng1))
		{
			T2 = Tx;
			dlng2 = dlngx;
		}
		else
		{
			T1 = Tx;
			dlng1 = dlngx;
		}
		iter++;

	} while (iter < 30);

	GMT_cross = Tx;
	return abs(dlngx);
}