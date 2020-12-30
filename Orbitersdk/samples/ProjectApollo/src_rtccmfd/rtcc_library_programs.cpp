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
	EphemerisDataTable EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;
	int err = ELFECH(GMT, 1, 1, L, EPHEM, MANTIMES, LUNSTAY);
	if (err == 0)
	{
		SV = EPHEM.table[0];
	}
	return err;
}

int RTCC::ELFECH(double GMT, unsigned vec_tot, unsigned vec_bef, int L, EphemerisDataTable &EPHEM, ManeuverTimesTable &MANTIMES, LunarStayTimesTable &LUNSTAY)
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
	EPHEM.Header.NumVec = EPHEM.table.size();
	EPHEM.Header.Offset = 0;
	EPHEM.Header.TL = EPHEM.table.front().GMT;
	EPHEM.Header.TR = EPHEM.table.back().GMT;

	return 0;
}

//TBD: ELGLCV

//Vector Count Routine
int RTCC::ELNMVC(double TL, double TR, int L, unsigned &NumVec, int &TUP)
{
	int err;
	OrbitEphemerisTable *tab;
	if (L == 1)
	{
		tab = &EZEPH1;
	}
	else
	{
		tab = &EZEPH2;
	}

	if (tab->EPHEM.table.size() == 0)
	{
		err = 32;
		goto RTCC_ELNMVC_2B;
	}
	TUP = abs(tab->EPHEM.Header.TUP);
	if (TUP == 0)
	{
		err = 64;
		goto RTCC_ELNMVC_2B;
	}

	tab->EPHEM.Header.TUP = -tab->EPHEM.Header.TUP;

	if (TL < tab->EPHEM.Header.TL)
	{
		if (TR < tab->EPHEM.Header.TL)
		{
			err = 128;
			goto RTCC_ELNMVC_2B;
		}
		TL = tab->EPHEM.Header.TL;
		err = 8;
	}
	if (TL > tab->EPHEM.Header.TR)
	{
		err = 128;
		goto RTCC_ELNMVC_2B;
	}
	double TREQ, T_Mid;
	unsigned LB, UB, Mid;
	bool firstpass = true;
	LB = 0;
	UB = tab->EPHEM.table.size() - 1;
	TREQ = TL;
RTCC_ELNMVC_1B:
	if (UB - LB <= 1)
	{
		goto RTCC_ELNMVC_1D;
	}
	Mid = (LB + UB) / 2;
	T_Mid = tab->EPHEM.table[Mid].GMT;
	if (TREQ < T_Mid)
	{
		UB = Mid;
		goto RTCC_ELNMVC_1B;
	}
	if (TREQ > T_Mid)
	{
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
int RTCC::ELVARY(EphemerisDataTable &EPH, unsigned ORER, double GMT, bool EXTRAP, EphemerisData &sv_out, unsigned &ORER_out)
{
	EphemerisData RES;
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

	//Reference body inconsistency
	if (EPH.table[i].RBI != EPH.table[i + ORER].RBI)
	{
		unsigned l;
		unsigned RBI_counter = 0;
		for (l = 0;l < ORER + 1;l++)
		{
			if (EPH.table[i + l].RBI == EPH.table[i].RBI)
			{
				RBI_counter++;
			}
		}
		if (RBI_counter > ORER + 1 - RBI_counter)
		{
			//Most SVs have the same reference as i
			RES.RBI = EPH.table[i].RBI;
			l = ORER;
			while (EPH.table[i + l].RBI != EPH.table[i].RBI)
			{
				ORER--;
				l--;
			}
		}
		else
		{
			//Most SVs have the same reference as i+ORER
			RES.RBI = EPH.table[i + ORER].RBI;
			l = 0;
			while (EPH.table[i + l].RBI != EPH.table[i + ORER].RBI)
			{
				ORER--;
				i++;
				l++;
			}
		}
	}
	else
	{
		RES.RBI = EPH.table[i].RBI;
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
int RTCC::ELVCNV(VECTOR3 vec, double GMT, int in, int out, VECTOR3 &vec_out)
{
	EphemerisData eph, sv_out;
	int err = 0;

	eph.R = vec;
	eph.V = _V(1, 0, 0);
	eph.GMT = GMT;

	err = ELVCNV(eph, in, out, sv_out);
	vec_out = sv_out.R;
	return err;
}

int RTCC::ELVCNV(EphemerisDataTable &svtab, int in, int out, EphemerisDataTable &svtab_out)
{
	EphemerisData sv, sv_out;
	svtab_out.table.clear();
	int err = 0;
	for (unsigned i = 0;i < svtab.table.size();i++)
	{
		sv = svtab.table[i];
		err = ELVCNV(sv, in, out, sv_out);
		if (err)
		{
			break;
		}
		svtab_out.table.push_back(sv_out);
	}
	//Store some ephemeris header data
	svtab_out.Header.CSI = out;
	svtab_out.Header.NumVec = svtab_out.table.size();
	svtab_out.Header.TL = svtab_out.table.front().GMT;
	svtab_out.Header.TR = svtab_out.table.back().GMT;
	svtab_out.Header.TUP = svtab.Header.TUP;
	svtab_out.Header.VEH = svtab.Header.VEH;
	return err;
}

int RTCC::ELVCNV(EphemerisData &sv, int in, int out, EphemerisData &sv_out)
{
	if (in == out)
	{
		sv_out = sv;
		return 0;
	}

	int err;

	sv_out = sv;

	//0 = ECI, 1 = ECT, 2 = MCI, 3 = MCT, 4 = EMP

	//ECI to/from ECT
	if ((in == 0 && out == 1) || (in == 1 && out == 0))
	{
		MATRIX3 Rot = OrbMech::GetRotationMatrix(BODY_EARTH, OrbMech::MJDfromGET(sv.GMT, GMTBASE));

		if (in == 0)
		{
			sv_out.R = rhtmul(Rot, sv.R);
			sv_out.V = rhtmul(Rot, sv.V);
		}
		else
		{
			sv_out.R = rhmul(Rot, sv.R);
			sv_out.V = rhmul(Rot, sv.V);
		}
	}
	//ECI to/from MCI
	else if ((in == 0 && out == 2) || (in == 2 && out == 0))
	{
		VECTOR3 R_EM, V_EM, R_ES;

		if (!OrbMech::PLEFEM(pzefem, OrbMech::MJDfromGET(sv.GMT, GMTBASE), R_EM, V_EM, R_ES)) return 1;

		if (in == 0)
		{
			sv_out.R = sv.R - R_EM;
			sv_out.V = sv.V - V_EM;
		}
		else
		{
			sv_out.R = sv.R + R_EM;
			sv_out.V = sv.V + V_EM;
		}
	}
	//MCI to/from MCT
	else if ((in == 2 && out == 3) || (in == 3 && out == 2))
	{
		MATRIX3 Rot = OrbMech::GetRotationMatrix(BODY_MOON, OrbMech::MJDfromGET(sv.GMT, GMTBASE));

		if (in == 2)
		{
			sv_out.R = rhtmul(Rot, sv.R);
			sv_out.V = rhtmul(Rot, sv.V);
		}
		else
		{
			sv_out.R = rhmul(Rot, sv.R);
			sv_out.V = rhmul(Rot, sv.V);
		}
	}
	//MCI to/from EMP
	else if ((in == 2 && out == 4) || (in == 4 && out == 2))
	{
		MATRIX3 Rot;
		VECTOR3 R_EM, V_EM, R_ES;
		VECTOR3 X_EMP, Y_EMP, Z_EMP;

		if (!OrbMech::PLEFEM(pzefem, OrbMech::MJDfromGET(sv.GMT, GMTBASE), R_EM, V_EM, R_ES)) return 1;

		X_EMP = -unit(R_EM);
		Z_EMP = unit(crossp(R_EM, V_EM));
		Y_EMP = crossp(Z_EMP, X_EMP);
		Rot = _M(X_EMP.x, X_EMP.y, X_EMP.z, Y_EMP.x, Y_EMP.y, Y_EMP.z, Z_EMP.x, Z_EMP.y, Z_EMP.z);

		if (in == 2)
		{
			sv_out.R = rhmul(Rot, sv.R);
			sv_out.V = rhmul(Rot, sv.V);
		}
		else
		{
			sv_out.R = rhtmul(Rot, sv.R);
			sv_out.V = rhtmul(Rot, sv.V);
		}
	}
	//ECI to MCT
	else if (in == 0 && out == 3)
	{
		EphemerisData sv1;

		err = ELVCNV(sv, 0, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 3, sv_out);
		if (err) return err;
	}
	//MCT to ECI
	else if (in == 3 && out == 0)
	{
		EphemerisData sv1;
		err = ELVCNV(sv, 3, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 0, sv_out);
		if (err) return err;
	}
	//ECI to EMP
	else if (in == 0 && out == 4)
	{
		EphemerisData sv1;

		err = ELVCNV(sv, 0, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 4, sv_out);
		if (err) return err;
	}
	//EMP to ECI
	else if (in == 4 && out == 0)
	{
		EphemerisData sv1;
		err = ELVCNV(sv, 4, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 0, sv_out);
		if (err) return err;
	}
	//ECT to EMP
	else if (in == 1 && out == 4)
	{
		EphemerisData sv1, sv2;

		err = ELVCNV(sv, 1, 0, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 0, 2, sv2);
		if (err) return err;
		err = ELVCNV(sv2, 2, 4, sv_out);
		if (err) return err;
	}
	//EMP to ECT
	else if (in == 4 && out == 1)
	{
		EphemerisData sv1, sv2;

		err = ELVCNV(sv, 4, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 0, sv2);
		if (err) return err;
		err = ELVCNV(sv2, 0, 1, sv_out);
		if (err) return err;
	}
	//MCT to EMP
	else if (in == 3 && out == 4)
	{
		EphemerisData sv1;
		err = ELVCNV(sv, 3, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 4, sv_out);
		if (err) return err;
	}
	//EMP to MCT
	else if (in == 4 && out == 3)
	{
		EphemerisData sv1;
		err = ELVCNV(sv, 4, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 3, sv_out);
		if (err) return err;
	}

	return 0;
}

//Extended Interpolation Routine
void RTCC::ELVCTR(const ELVCTRInputTable &in, ELVCTROutputTable &out)
{
	EphemerisDataTable EPHEM;
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

void RTCC::ELVCTR(const ELVCTRInputTable &in, ELVCTROutputTable &out, EphemerisDataTable &EPH, ManeuverTimesTable &mantimes, LunarStayTimesTable *LUNRSTAY)
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
		while (TE != EPH.table[NV - 1].GMT)
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

//Subsatellite position
int RTCC::GLSSAT(EphemerisData sv, double &lat, double &lng, double &alt)
{
	EphemerisData sv_out;
	VECTOR3 u;
	int in, out;
	if (sv.RBI == BODY_EARTH)
	{
		in = 0;
		out = 1;
	}
	else
	{
		in = 2;
		out = 3;
	}

	if (ELVCNV(sv, in, out, sv_out))
	{
		return 1;
	}
	u = unit(sv.R);
	lat = atan2(u.z, sqrt(u.x*u.x + u.y*u.y));
	lng = atan2(u.y, u.x);
	if (sv.RBI == BODY_EARTH)
	{
		alt = length(sv.R) - OrbMech::R_Earth;
	}
	else
	{
		alt = length(sv.R) - BZLAND.rad[RTCC_LMPOS_BEST];
	}
	return 0;
}

//Weight Access Routine
int RTCC::PLAWDT(int L, double gmt, double &cfg_weight)
{
	double csm_weight, lma_weight, lmd_weight, sivb_weight;
	std::bitset<4> cfg;

	return PLAWDT(L, gmt, cfg, cfg_weight, csm_weight, lma_weight, lmd_weight, sivb_weight);
}

int RTCC::PLAWDT(int L, double gmt, std::bitset<4> &cfg, double &cfg_weight, double &csm_weight, double &lm_asc_weight, double &lm_dsc_weight, double &sivb_weight)
{
	MissionPlanTable *table = GetMPTPointer(L);
	unsigned i = 0;

	//No maneuver in MPT or time is before first maneuver. Use initial values
	if (table->mantable.size() == 0 || gmt <= table->mantable[0].GMT_BI)
	{
		cfg = table->CommonBlock.ConfigCode;
		cfg_weight = table->TotalInitMass;
		if (cfg[RTCC_CONFIG_C])
		{
			csm_weight = table->CommonBlock.CSMMass;
		}
		else
		{
			csm_weight = 0.0;
		}
		if (cfg[RTCC_CONFIG_A])
		{
			lm_asc_weight = table->CommonBlock.LMAscentMass;
		}
		else
		{
			lm_asc_weight = 0.0;
		}
		if (cfg[RTCC_CONFIG_D])
		{
			lm_dsc_weight = table->CommonBlock.LMDescentMass;
		}
		else
		{
			lm_dsc_weight = 0.0;
		}
		if (cfg[RTCC_CONFIG_S])
		{
			sivb_weight = table->CommonBlock.SIVBMass;
		}
		else
		{
			sivb_weight = 0.0;
		}
		return 0;
	}

	//Iterate to find maneuver
	while ((i < table->mantable.size() - 1) && (gmt >= table->mantable[i + 1].GMT_BO))
	{
		i++;
	}

	cfg = table->mantable[i].CommonBlock.ConfigCode;
	cfg_weight = table->mantable[i].TotalMassAfter;
	csm_weight = table->mantable[i].CommonBlock.CSMMass;
	lm_asc_weight = table->mantable[i].CommonBlock.LMAscentMass;
	lm_dsc_weight = table->mantable[i].CommonBlock.LMDescentMass;
	sivb_weight = table->mantable[i].CommonBlock.SIVBMass;

	return 0;
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
double RTCC::RLMTLC(EphemerisDataTable &ephemeris, ManeuverTimesTable &MANTIMES, double long_des, double GMT_min, double &GMT_cross, LunarStayTimesTable *LUNRSTAY)
{
	if (ephemeris.Header.TUP <= 0) return -1.0;
	if (ephemeris.table.size() == 0) return -1.0;

	ELVCTRInputTable interin;
	ELVCTROutputTable interout;
	EphemerisData sv_cur;
	double T1, lat, lng, dlng1, T2, dlng2, Tx, dlngx;
	int RBI_search;
	unsigned i = 0;

	if (GMT_min > ephemeris.table.front().GMT)
	{
		if (GMT_min > ephemeris.table.back().GMT)
		{
			return -1.0;
		}
		T1 = GMT_min;
	}
	else
	{
		T1 = ephemeris.table.front().GMT;
	}

	while (T1 > ephemeris.table[i + 1].GMT)
	{
		i++;
	}

	interin.GMT = T1;
	ELVCTR(interin, interout, ephemeris, MANTIMES, LUNRSTAY);

	if (interout.ErrorCode)
	{
		return -1.0;
	}

	RBI_search = interout.SV.RBI;
	sv_cur = interout.SV;
	OrbMech::latlong_from_J2000(sv_cur.R, OrbMech::MJDfromGET(sv_cur.GMT, GMTBASE), sv_cur.RBI, lat, lng);
	dlng1 = lng - long_des;
	if (dlng1 > PI) { dlng1 -= PI2; }
	else if (dlng1 < -PI) { dlng1 += PI2; }

	if (abs(dlng1) < 0.0001)
	{
		GMT_cross = T1;
		return 0;
	}

	if (i >= ephemeris.table.size() - 1)
	{
		return -1.0;
	}

	while (i < ephemeris.table.size() - 1)
	{
		i++;
		sv_cur = ephemeris.table[i];
		//Reference body inconsistency, abort
		if (sv_cur.RBI != RBI_search)
		{
			return -1.0;
		}
		T2 = sv_cur.GMT;

		OrbMech::latlong_from_J2000(sv_cur.R, OrbMech::MJDfromGET(sv_cur.GMT, GMTBASE), sv_cur.RBI, lat, lng);
		dlng2 = lng - long_des;
		if (dlng2 > PI) { dlng2 -= PI2; }
		else if (dlng2 < -PI) { dlng2 += PI2; }

		if (abs(dlng2) < 0.0001)
		{
			GMT_cross = T2;
			return 0;
		}

		if (dlng1*dlng2 < 0 && abs(dlng1 - dlng2) < PI)
		{
			break;
		}
		if (i == ephemeris.table.size() - 1)
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
		ELVCTR(interin, interout, ephemeris, MANTIMES, LUNRSTAY);

		if (interout.ErrorCode)
		{
			return -1.0;
		}
		//Reference body inconsistency, abort
		if (interout.SV.RBI != RBI_search)
		{
			return -1.0;
		}

		sv_cur = interout.SV;
		OrbMech::latlong_from_J2000(sv_cur.R, OrbMech::MJDfromGET(sv_cur.GMT, GMTBASE), sv_cur.RBI, lat, lng);
		dlngx = lng - long_des;
		if (dlngx > PI) { dlngx -= PI2; }
		else if (dlngx < -PI) { dlngx += PI2; }

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