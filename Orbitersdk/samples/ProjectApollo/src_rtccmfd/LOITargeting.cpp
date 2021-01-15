/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2020

Lunar Orbit Insertion Targeting

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

#include "LOITargeting.h"
#include "rtcc.h"
#include "OrbMech.h"

namespace rtcc
{

	LOITargeting::LOITargeting(RTCC *r, LOIOptions o) : RTCCModule(r)
	{
		opt = o;
		hMoon = oapiGetObjectByName("Moon");
	}

	bool LOITargeting::MAIN()
	{
		MATRIX3 Rot;
		double psi_XTRA;

		RA_LPO = opt.HA_LPO + opt.R_LLS;
		RP_LPO = opt.HP_LPO + opt.R_LLS;
		R_PHYP = length(opt.SPH.R);
		U_H = unit(crossp(opt.SPH.R, opt.SPH.V));
		U_PC = unit(opt.SPH.R);
		a_H = 1.0 / (2.0 / R_PHYP - pow(length(opt.SPH.V), 2) / OrbMech::mu_Moon);
		e_H = 1.0 - R_PHYP / a_H;

		if (opt.R_LLS > R_PHYP || R_PHYP > RA_LPO)
		{
			return true;
		}
		if (opt.psi_mx < opt.psi_DS || opt.psi_DS < opt.psi_mn)
		{
			return true;
		}
		psi_XTRA = opt.psi_mx - 5.0*RAD;
		if (opt.psi_mx - opt.psi_mn >= 170.0*RAD)
		{
			return true;
		}
		if (RA_LPO < RP_LPO)
		{
			return true;
		}

		//Compute LPO at LLS
		double a_LLS, e_LLS, P_LLS, R, V, cos_gamma, T_LLS, gamma;
		a_LLS = (opt.HA_LLS + opt.HP_LLS) / 2.0 + opt.R_LLS;
		e_LLS = (opt.HA_LLS + opt.R_LLS) / a_LLS - 1.0;
		P_LLS = a_LLS * (1.0 - e_LLS * e_LLS);
		R = P_LLS / (1.0 + e_LLS * cos(opt.DW));
		V = sqrt(OrbMech::mu_Moon*(2.0 / R - 1.0 / a_LLS));
		cos_gamma = sqrt(OrbMech::mu_Moon*a_LLS*(1.0 - e_LLS * e_LLS)) / (R*V);
		if (abs(cos_gamma) > 1.0)
		{
			cos_gamma = cos_gamma / abs(cos_gamma);
		}
		gamma = -OrbMech::sign(opt.DW)*acos(cos_gamma);
		VECTOR3 R_LLS_sg, V_LLS_sg, R_LLS, V_LLS, R_LOI, V_LOI, R_LOI2, V_LOI2, U_DS, U_XTRA, NU, U_MN, U_MX, U, R_N, V_N, U_S, U_PL, U_L;
		double DA, eta2, theta_cop, r_N, R_P, dt, cos_dpsi_N, theta, dv_p, dv_m, dt3, dh_a, dh_p, dw_p, a_L, e_L;

		T_LLS = TIME(a_LLS, e_LLS, opt.lng_LLS, opt.lat_LLS, DA, eta2, dt3);
		OrbMech::adbar_from_rv(R, V, opt.lng_LLS, opt.lat_LLS, PI05 - gamma, opt.psi_DS, R_LLS_sg, V_LLS_sg);
		bool firstpass = true;
	LOI_MAIN_BACKUP:
		Rot = OrbMech::GetRotationMatrix(BODY_MOON, OrbMech::MJDfromGET(T_LLS, opt.GMTBASE));
		R_LLS = rhmul(Rot, R_LLS_sg);
		V_LLS = rhmul(Rot, V_LLS_sg);
		BACKUP(R_LLS, V_LLS, a_LLS, T_LLS, DA, eta2, dt3, dh_a, dh_p, dw_p, a_L, e_L, U_PL, R_LOI, V_LOI);
		if (firstpass)
		{
			firstpass = false;
			T_LLS = T_LLS + DT_CORR;
			goto LOI_MAIN_BACKUP;
		}

		out.T_LLS = T_LLS;

		//Save the LPO-2, dh_a, dh_p, dw_p computed in backup for use later in the program
		ALSAV = a_L;
		ELSAV = e_L;
		UPLSAV = U_PL;
		DHASAV = dh_a;
		DHPSAV = dh_p;
		DWPSAV = dw_p;
		U_DS = unit(crossp(R_LOI, V_LOI));

		OrbMech::adbar_from_rv(R, V, opt.lng_LLS, opt.lat_LLS, PI05 - gamma, psi_XTRA, R_LLS_sg, V_LLS_sg);
		Rot = OrbMech::GetRotationMatrix(BODY_MOON, OrbMech::MJDfromGET(T_LLS, opt.GMTBASE));
		R_LLS = rhmul(Rot, R_LLS_sg);
		V_LLS = rhmul(Rot, V_LLS_sg);
		BACKUP(R_LLS, V_LLS, a_LLS, T_LLS, DA, eta2, dt3, dh_a, dh_p, dw_p, a_L, e_L, U_PL, R_LOI2, V_LOI2);
		U_XTRA = unit(crossp(R_LOI2, V_LOI2));
		U_N = unit(crossp(U_DS, U_XTRA));
		NU = unit(crossp(U_N, U_DS));
		U_MN = U_DS * cos(opt.psi_DS - opt.psi_mn) + NU * sin(opt.psi_DS - opt.psi_mn);
		U_MX = U_DS * cos(opt.psi_mx - opt.psi_DS) - NU * sin(opt.psi_mx - opt.psi_DS);
		U = U_DS;
		theta_cop = acos(dotp(U, U_H));
		COPLNR(U_DS);

		if (1.0 - dotp(U, U_H) >= 0.001*RAD)
		{
			goto LOI_MAIN_ONE_NODE;
		}

		//Store +/- coplanar solns as +/- plane solns
		out.data[6] = out.data[2];
		out.data[7] = out.data[3];

		INTER(out.data[6].R_LOI, out.data[6].V_LOI, out.data[6].GMT_LOI, UPLSAV, out.data[6].USSAV, true);
		INTER(out.data[7].R_LOI, out.data[7].V_LOI, out.data[7].GMT_LOI, UPLSAV, out.data[7].USSAV, false);
		goto LOI_MAIN_DISPLAY;
	LOI_MAIN_ONE_NODE:
		NU = unit(crossp(U, U_H));
		if (dotp(NU, U_PC) < 0)
		{
			NU = -NU;
		}
		r_N = a_H * (1.0 - e_H * e_H) / (1.0 + e_H * dotp(NU, U_PC));
		if (r_N > RA_LPO)
		{
			goto LOI_MAIN_MIN_THETA;
		}
		//Compute conic hyperbolic state vector at node
		double sgn;
		if (dotp(crossp(U_PC, NU), U_H) > 0.0)
		{
			sgn = 1.0;
		}
		else
		{
			sgn = -1.0;
		}

		dt = OrbMech::time_radius(opt.SPH.R, opt.SPH.V*sgn, r_N, 1.0, OrbMech::mu_Moon);
		dt *= sgn;
		OrbMech::rv_from_r0v0(opt.SPH.R, opt.SPH.V, dt, R_N, V_N, OrbMech::mu_Moon);
		if (r_N < RP_LPO)
		{
			R_P = r_N;
		}
		else
		{
			R_P = RP_LPO;
		}
		cos_dpsi_N = dotp(U, U_H);
		theta = 0.0;
		U_S = U_DS;
		//Call DVDA with cos_dpsi_N, R_P and state vector at node for DV+ of + plane soln
		dv_p = DVDA(R_N, V_N, 1.0, true, cos_dpsi_N, 0.0);
		//Call CANS to store + plane soln
		CANS(R_N, V_N, opt.SPH.GMT + dt, U_S, R_P, theta, 1.0, 7);
		//Call DVDA with cos_dpsi_N, R_P and state vector at node for DV- of - plane soln
		dv_m = DVDA(R_N, V_N, -1.0, true, cos_dpsi_N, 0.0);
		CANS(R_N, V_N, opt.SPH.GMT + dt, U_S, R_P, theta, -1.0, 8);
		int MT;
		MT = 0;
		//Store plane solutions as min theta solutions
		out.data[4] = out.data[6];
		out.data[5] = out.data[7];
		if (r_N < RP_LPO)
		{
			MT = 1;
			//Call MINTHT for +/- min theta solns
			MINTHT(U);
		}
		else
		{
			if (dv_p > opt.DV_maxp)
			{
				MT = 1;
				if (dv_m > opt.DV_maxm)
				{
					//Call MINTHT for +/- min theta soln
					MINTHT(U);
				}
				else
				{
					//Call MINTHT for + min theta soln
					MINTHT(U);
				}
			}
			else
			{
				if (dv_m > opt.DV_maxm)
				{
					//Call MINTHT for - min theta soln
					MINTHT(U);
					MT = 1;
				}
			}
		}
		if (MT == 0 || opt.usePlaneSolnForInterSoln)
		{
			INTER(out.data[6].R_LOI, out.data[6].V_LOI, out.data[6].GMT_LOI, UPLSAV, U_DS, true);
			INTER(out.data[7].R_LOI, out.data[7].V_LOI, out.data[7].GMT_LOI, UPLSAV, U_DS, false);
			goto LOI_MAIN_DISPLAY;
		}
		else
		{
			goto LOI_MAIN_A;
		}
	LOI_MAIN_MIN_THETA:
		//Compute display parameters at the node for the +/- plane solns
		out.data[6].display.eta_N = OrbMech::sign(dotp(crossp(U_PC, out.data[6].R_LOI), U_H))*acos(dotp(U_PC, unit(out.data[6].R_LOI)));
		out.data[7].display.eta_N = OrbMech::sign(dotp(crossp(U_PC, out.data[7].R_LOI), U_H))*acos(dotp(U_PC, unit(out.data[7].R_LOI)));
		out.data[6].display.H_ND = length(out.data[6].R_LOI) - opt.R_LLS;
		out.data[7].display.H_ND = length(out.data[7].R_LOI) - opt.R_LLS;
		MINTHT(U_DS);
	LOI_MAIN_A:
		VECTOR3 U_Sp, U_Sm;
		U_Sp = out.data[4].USSAV;
		U_Sm = out.data[5].USSAV;
		U_L = unit(UPLSAV - U_Sp * dotp(UPLSAV, U_Sp));
		INTER(out.data[4].R_LOI, out.data[4].V_LOI, out.data[4].GMT_LOI, U_L, U_Sp, true);
		U_L = unit(UPLSAV - U_Sm * dotp(UPLSAV, U_Sm));
		INTER(out.data[5].R_LOI, out.data[5].V_LOI, out.data[5].GMT_LOI, U_L, U_Sm, true);
	LOI_MAIN_DISPLAY:
		VECTOR3 NU_MX, NU_MN, NU1, NU2;
		double eta_MX, eta_MN, eta_DS;

		NU_MX = unit(crossp(U_MX, U_H));
		NU_MN = unit(crossp(U_MN, U_H));
		eta_MX = OrbMech::sign(dotp(crossp(U_PC, NU_MX), U_H))*acos(dotp(U_PC, NU_MX));
		eta_MN = OrbMech::sign(dotp(crossp(U_PC, NU_MN), U_H))*acos(dotp(U_PC, NU_MN));
		eta_DS = out.data[7].display.eta_N;
		if (opt.psi_DS = opt.psi_mn && opt.psi_DS == opt.psi_mx)
		{
			eta_MX = eta_MN = eta_DS;
		}
		else
		{
			if (opt.psi_mn == opt.psi_DS)
			{
				NU1 = NU_MX;
				NU2 = NU_MN;
			}
			else
			{
				NU1 = NU_MN;
				NU2 = NU_MX;
			}
			if (dotp(crossp(NU1, NU2), crossp(NU1, unit(out.data[7].R_LOI))) <= 0)
			{
				NU_MX = -NU_MX;
				NU_MN = -NU_MN;
				eta_MX = OrbMech::sign(dotp(crossp(U_PC, NU_MX), U_H))*acos(dotp(U_PC, NU_MX));
				eta_MN = OrbMech::sign(dotp(crossp(U_PC, NU_MN), U_H))*acos(dotp(U_PC, NU_MN));
			}
		}
		out.eta_MN = eta_MN;
		out.eta_MX = eta_MX;
		double h_NMX, h_NMN, e;
		h_NMX = a_H * (1.0 - e_H * e_H) / (1.0 + e_H * dotp(NU_MX, U_PC)) - opt.R_LLS;
		h_NMN = a_H * (1.0 - e_H * e_H) / (1.0 + e_H * dotp(NU_MN, U_PC)) - opt.R_LLS;
		//Compute R_p to give eta1 at the node for each intersection soln (for possible display)
		for (int i = 0;i < 2;i++)
		{
			e = (RA_LPO - length(out.data[i].R_LOI)) / (length(out.data[i].R_LOI)*cos(opt.eta1) + RA_LPO);
			R_P = 2.0*RA_LPO / (e + 1.0) - RA_LPO;
		}

		EphemerisData sv_H;
		VECTOR3 V_A, u;
		double gamma_H, r_H, psi_H, v_A, psi_A, gamma_A, decl, rtasc;
		int ITS;

		for (int i = 0;i < 8;i++)
		{
			//If intersection solution wasn't calculated, skip
			if (out.data[i].GMT_LOI == 0.0)
			{
				continue;
			}
			out.data[i].display.dv_LOI2 = DELV2(out.data[i].R_LOI, out.data[i].USSAV, RA_LPO + DHASAV, out.data[i].display.h_P + opt.R_LLS + DHPSAV, out.data[i].display.W_P);
			sgn = OrbMech::sign(dotp(crossp(U_PC, out.data[i].R_LOI), U_H));

			pRTCC->PMMCEN(opt.SPH, 0.0, 24.0*3600.0, 3, length(out.data[i].R_LOI), sgn, sv_H, ITS);

			gamma_H = PI05 - acos2(dotp(unit(sv_H.R), unit(sv_H.V)));
			r_H = length(sv_H.R);
			psi_H = atan2(sv_H.R.x*sv_H.V.y - sv_H.R.y*sv_H.V.x, sv_H.V.z*r_H - sv_H.R.z*dotp(sv_H.R, sv_H.V) / r_H);
			if (psi_H < 0)
			{
				psi_H += PI2;
			}
			v_A = length(sv_H.V) + out.data[i].dv_LOI;
			psi_A = psi_H + out.data[i].dpsi_LOI;
			gamma_A = gamma_H + out.data[i].dgamma_LOI;
			u = unit(sv_H.R);
			decl = atan2(u.z, sqrt(u.x*u.x + u.y*u.y));
			rtasc = atan2(u.y, u.x);
			gamma_A = PI05 - gamma_A;
			V_A.x = v_A * (cos(rtasc)*(-cos(psi_A)*sin(gamma_A)*sin(decl) + cos(gamma_A)*cos(decl)) - sin(psi_A)*sin(gamma_A)*sin(rtasc));
			V_A.y = v_A * (sin(rtasc)*(-cos(psi_A)*sin(gamma_A)*sin(decl) + cos(gamma_A)*cos(decl)) + sin(psi_A)*sin(gamma_A)*cos(rtasc));
			V_A.z = v_A * (cos(psi_A)*cos(decl)*sin(gamma_A) + cos(gamma_A)*sin(decl));
			out.data[i].R_LOI = sv_H.R;
			out.data[i].V_LOI = sv_H.V;
			out.data[i].V_LOI_apo = V_A;
			out.data[i].GMT_LOI = sv_H.GMT;
		}

		return false;
	}

	double LOITargeting::TIME(double a_LLS, double e_LLS, double lng_LLS, double lat_LLS, double &DA, double &eta2, double &dt3)
	{
		bool recycle = false;
		VECTOR3 U_LLS_sg, U_LLS;
		double R1, DR1, R2, dt1, dt2, T_LLS, a1, e1, da;

		DR1 = modf(opt.REVS1, &R1)*PI2;
		R2 = (double)opt.REVS2;
		a1 = (RA_LPO + RP_LPO) / 2.0;
		e1 = RA_LPO / a1 - 1.0;
		dt2 = DELTAT(a1, e1, opt.eta1, DR1);
		eta2 = opt.eta1 + DR1;
		while (eta2 < -PI)
		{
			eta2 += PI2;
		}
		while (eta2 > PI)
		{
			eta2 -= PI2;
		}

		dt1 = R1 * PI2*sqrt(pow(a1, 3) / OrbMech::mu_Moon) + R2 * PI2*sqrt(pow(a_LLS, 3) / OrbMech::mu_Moon);
		T_LLS = opt.SPH.GMT + dt1 + dt2;
		U_LLS_sg = OrbMech::r_from_latlong(opt.lat_LLS, opt.lng_LLS);

	LOI_TIME_A:
		U_LLS = rhmul(OrbMech::GetRotationMatrix(BODY_MOON, opt.GMTBASE + T_LLS / 24.0 / 3600.0), U_LLS_sg);
		da = acos(dotp(U_LLS, U_PC));
		if (dotp(crossp(U_LLS, U_PC), U_H) > 0)
		{
			da = PI2 - da;
		}
		if (da > DR1)
		{
			DA = da - DR1;
		}
		else
		{
			DA = PI2 - (DR1 - da);
		}
		dt3 = DELTAT(a_LLS, e_LLS, opt.DW < 0 ? -opt.DW : PI2 - opt.DW, -DA);
		T_LLS = opt.SPH.GMT + dt1 + dt2 - dt3;
		if (recycle == false)
		{
			recycle = true;
			goto LOI_TIME_A;
		}
		return T_LLS;
	}

	void LOITargeting::BACKUP(VECTOR3 R_LLS, VECTOR3 V_LLS, double a_LLS, double T_LLS, double DA, double eta2, double dt3, double &dh_a, double &dh_p, double &dw_p, double &a_L, double &e_L, VECTOR3 &U_PL, VECTOR3 &R_LOI, VECTOR3 &V_LOI)
	{
		OELEMENTS coe;
		VECTOR3 R, V, R_apo, U_LLS, U, R2, V2;
		double T, a1, a, e, eta, deta, dt, T2, R_P;

		U_LLS = unit(R_LLS);
		T = T_LLS + dt3 - ((double)opt.REVS2) * PI2*sqrt(pow(a_LLS, 3) / OrbMech::mu_Moon);
		OrbMech::oneclickcoast(R_LLS, V_LLS, OrbMech::MJDfromGET(T_LLS, opt.GMTBASE), T - T_LLS, R, V, hMoon, hMoon);
		R_apo = unit(R);

		coe = OrbMech::coe_from_sv(R, V, OrbMech::mu_Moon);
		a = OrbMech::GetSemiMajorAxis(R, V, OrbMech::mu_Moon);
		e = coe.e;
		eta = coe.TA;

		a1 = acos(dotp(R_apo, U_LLS));
		U = unit(crossp(R, V));
		if (dotp(crossp(R_apo, U_LLS), U) <= 0)
		{
			a1 = PI2 - a1;
		}
		deta = a1 - DA;
		dt = DELTAT(a, e, eta, deta);
		OrbMech::oneclickcoast(R, V, OrbMech::MJDfromGET(T, opt.GMTBASE), dt, R2, V2, hMoon, hMoon);
		T2 = T + dt;

		coe = OrbMech::coe_from_sv(R2, V2, OrbMech::mu_Moon);
		a_L = OrbMech::GetSemiMajorAxis(R2, V2, OrbMech::mu_Moon);
		e_L = coe.e;
		U_PL = unit(crossp(V2, crossp(R2, V2)) / OrbMech::mu_Moon - R2 / length(R2));
		R_P = a * (1.0 - e);
		if (R_P < length(R_LLS))
		{
			//Error
		}
		//TBD: Integrate backwards one period from this point to compute an(d?) LPO for display or on-line print
		//Compute first LPO
		VECTOR3 U_P2, U_PJ, U_P1;
		double r, v, rtasc, decl, fpatemp, az, p, cos_gamma, gamma, R_A2, R_P2, dt_LOI_DOI, R_A1, R_P1;
		OrbMech::rv_from_adbar(R2, V2, r, v, rtasc, decl, fpatemp, az);
		a = (RA_LPO + RP_LPO) / 2.0;
		e = RA_LPO / a - 1.0;
		p = a * (1.0 - e * e);
		r = p / (1.0 + e * cos(eta2));
		v = sqrt(OrbMech::mu_Moon*(2.0 / r - 1.0 / a));
		cos_gamma = sqrt(OrbMech::mu_Moon*p) / r / v;
		T = T2;
		if (cos_gamma > 1.0)
		{
			cos_gamma = 1.0;
		}
		gamma = OrbMech::sign(eta2)*acos(cos_gamma);
		R_A2 = RA_LPO;
		R_P2 = RP_LPO;
		OrbMech::adbar_from_rv(r, v, rtasc, decl, PI05 - gamma, az, R, V);
		U_P2 = unit(crossp(V, crossp(R, V)) / OrbMech::mu_Moon - R / length(R));
		OrbMech::oneclickcoast(R, V, OrbMech::MJDfromGET(T, opt.GMTBASE), opt.SPH.GMT - T, R_LOI, V_LOI, hMoon, hMoon);
		coe = OrbMech::coe_from_sv(R_LOI, V_LOI, OrbMech::mu_Moon);
		a = OrbMech::GetSemiMajorAxis(R_LOI, V_LOI, OrbMech::mu_Moon);
		e = coe.e;
		eta = coe.TA;
		U = unit(crossp(R_LOI, V_LOI));
		U_PJ = unit(U_PC - U * dotp(U_PC, U));
		deta = OrbMech::sign(dotp(crossp(U_PJ, unit(R_LOI)), U))*acos(dotp(U_PJ, unit(R_LOI)));
		DT_CORR = DELTAT(a, e, eta, deta);
		dt_LOI_DOI = T2 - opt.SPH.GMT;
		OrbMech::periapo(R_LOI, V_LOI, OrbMech::mu_Moon, R_A1, R_P1);
		U_P1 = unit(crossp(V_LOI, crossp(R_LOI, V_LOI)) / OrbMech::mu_Moon - R_LOI / length(R_LOI));
		if (R_P1 < (length(R_LLS) + 10.0*1852.0))
		{
			//Error
		}
		dh_a = R_A2 - R_A1;
		dh_p = R_P2 - R_P1;
		dw_p = OrbMech::sign(dotp(crossp(U_P1, U_P2), U))*acos(dotp(U_P1, U_P2));
	}

	double LOITargeting::DELTAT(double a, double e, double eta, double deta)
	{
		double K_e, E, eta_apo, dE, E_apo, DE, DM;

		if (eta < 0)
		{
			eta += PI2;
		}
		K_e = sqrt((1.0 - e) / (1.0 + e));
		E = atan2(sqrt(1.0 - e * e)*sin(eta), cos(eta) + e);
		if (E < 0)
		{
			E += PI2;
		}
		eta_apo = eta + deta;
		if (eta_apo >= 0 && eta_apo < PI2)
		{
			dE = 0.0;
		}
		else
		{
			dE = 0.0;
			if (eta_apo < 0)
			{
				while (eta_apo < 0)
				{
					eta_apo += PI2;
					dE += PI2;
				}
				dE = -(dE); //plus PI2???
			}
			else
			{
				while (eta_apo >= PI2)
				{
					eta_apo -= PI2;
					dE += PI2;
				}
			}
		}
		E_apo = atan2(sqrt(1.0 - e * e)*sin(eta_apo), cos(eta_apo) + e);
		while (E_apo < 0)
		{
			E_apo += PI2;
		}
		while (E_apo >= PI2)
		{
			E_apo -= PI2;
		}
		DE = E_apo - E + dE;
		DM = DE + e * (sin(E) - sin(E_apo));
		return sqrt(pow(a, 3) / OrbMech::mu_Moon)*DM;
	}

	void LOITargeting::COPLNR(VECTOR3 U_DS)
	{
		VECTOR3 U_S, R_N, V_N;
		double R, DV_copp, cos_dpsi_N, theta_cop, dt, DV_p;
		DV_copp = 10000.0*0.3048;
		if (R_PHYP >= RP_LPO)
		{
			R = R_PHYP - 0.01*1852.0;
		}
		else
		{
			R = RP_LPO - 0.01*1862.0;
		}
		theta_cop = acos(dotp(U_DS, U_H));
		U_S = U_H;
		cos_dpsi_N = 1.0;
	LOI_COPLNR_1:
		R = R + 0.01*1852.0;
		if (R > RA_LPO)
		{
			goto LOI_COPLNR_2;
		}
		dt = -OrbMech::time_radius(opt.SPH.R, -opt.SPH.V, R, 1.0, OrbMech::mu_Moon);
		OrbMech::rv_from_r0v0(opt.SPH.R, opt.SPH.V, dt, R_N, V_N, OrbMech::mu_Moon);
		DV_p = DVDA(R_N, V_N, 1.0, true, cos_dpsi_N, 0.0);
		if (DV_p < DV_copp)
		{
			DV_copp = DV_p;
			goto LOI_COPLNR_1;
		}
	LOI_COPLNR_2:
		//Call CANS with pre-hyperbolic perilune state at R-0.01NM for + soln
		dt = -OrbMech::time_radius(opt.SPH.R, -opt.SPH.V, R - 0.01*1852.0, 1.0, OrbMech::mu_Moon);
		OrbMech::rv_from_r0v0(opt.SPH.R, opt.SPH.V, dt, R_N, V_N, OrbMech::mu_Moon);
		CANS(R_N, V_N, opt.SPH.GMT + dt, U_S, RP_LPO, theta_cop, 1.0, 3);

		//Call CANS with post-hyperbolic perilune state at R-0.01NM for - soln (DV_copm == DV_copp)
		dt = OrbMech::time_radius(opt.SPH.R, opt.SPH.V, R - 0.01*1852.0, 1.0, OrbMech::mu_Moon);
		OrbMech::rv_from_r0v0(opt.SPH.R, opt.SPH.V, dt, R_N, V_N, OrbMech::mu_Moon);
		CANS(R_N, V_N, opt.SPH.GMT + dt, U_S, RP_LPO, theta_cop, -1.0, 4);
	}

	double LOITargeting::DVDA(VECTOR3 R_N, VECTOR3 V_N, double soln, bool dv, double cos_dpsi_N, double DV)
	{
		double SGN, gamma_H, r_N, v_H;

		r_N = length(R_N);
		v_H = length(V_N);
		gamma_H = PI05 - acos2(dotp(unit(R_N), unit(V_N)));
		SGN = -OrbMech::sign(gamma_H*soln);
		double R_p, a_L, e_L, V_L, H_L, cos_gamma_L, sin_gamma_L;
		R_p = RP_LPO;
		a_L = (RA_LPO + R_p) / 2.0;
		e_L = (RA_LPO / a_L - 1.0);
		V_L = sqrt(OrbMech::mu_Moon*(2.0 / r_N - 1.0 / a_L));
		H_L = sqrt(OrbMech::mu_Moon*a_L*(1.0 - e_L * e_L));
		cos_gamma_L = H_L / r_N / V_L;
		if (cos_gamma_L > 1.0)
		{
			cos_gamma_L = 1.0;
		}
		sin_gamma_L = sqrt(1.0 - cos_gamma_L * cos_gamma_L);
		if (dv)
		{
			return sqrt(v_H*v_H + V_L * V_L - 2.0*v_H*V_L*(cos(abs(gamma_H))*cos_gamma_L*cos_dpsi_N + SGN * sin(gamma_H)*sin_gamma_L));
		}
		else
		{
			return ((V_L*V_L + v_H * v_H - DV * DV) / (2.0*V_L*v_H) - SGN * sin(gamma_H)*sin_gamma_L) / (cos_gamma_L*cos(gamma_H));
		}
	}

	void LOITargeting::CANS(VECTOR3 R_N, VECTOR3 V_N, double T_N, VECTOR3 U_S, double R_P, double theta, double SGN, int soln)
	{
		VECTOR3 V_proj;
		double r_N, v_LPO, cos_gamma, DV, psi_LPO, dpsi, dgamma, gamma_LPO, v_H, gamma_H, psi_H, h_N, eta_N, delta_op, cos_wp, a, e, W_P;

		//Calculate parameters on hyperbola/node
		r_N = length(R_N);
		v_H = length(V_N);
		gamma_H = PI05 - acos2(dotp(unit(R_N), unit(V_N)));
		psi_H = atan2(R_N.x*V_N.y - R_N.y*V_N.x, V_N.z*r_N - R_N.z*dotp(R_N, V_N) / r_N);
		if (psi_H < 0)
		{
			psi_H += PI2;
		}

		v_LPO = sqrt(OrbMech::mu_Moon*(2.0 / r_N - 2.0 / (RA_LPO + R_P)));
		cos_gamma = sqrt(OrbMech::mu_Moon*(RA_LPO + R_P)*(1.0 - pow(2.0*RA_LPO / (RA_LPO + R_P) - 1.0, 2))) / sqrt(2.0) / r_N / v_LPO;
		if (abs(cos_gamma) > 1)
		{
			cos_gamma = 1.0*OrbMech::sign(cos_gamma);
		}
		gamma_LPO = acos(cos_gamma);
		V_proj = unit(crossp(U_S, R_N));
		psi_LPO = atan2(R_N.x*V_proj.y - R_N.y*V_proj.x, V_proj.z*r_N - R_N.z*dotp(R_N, V_proj) / r_N);
		if (psi_LPO < 0)
		{
			psi_LPO += PI2;
		}

		DV = v_LPO - v_H;
		dpsi = psi_LPO - psi_H;
		dgamma = -SGN * gamma_LPO - gamma_H;

		h_N = r_N - opt.R_LLS;
		eta_N = OrbMech::sign(dotp(crossp(U_PC, unit(R_N)), U_H))*acos2(dotp(U_PC, unit(R_N)));
		delta_op = abs(acos(dotp(U_S, U_N)) - PI05);

		a = (RA_LPO + R_P) / 2.0;
		e = RA_LPO / a - 1.0;

		if (RA_LPO - R_P > 0.01*1852.0)
		{
			cos_wp = (a*(1.0 - e * e) / r_N - 1.0) / e;
		}
		else
		{
			cos_wp = 1.0;
		}
		if (abs(cos_wp) > 1.0)
		{
			cos_wp = cos_wp / abs(cos_wp);
		}
		W_P = -SGN * acos(cos_wp);

		//Store: state at node (R,V,T), dv, dgamma, dpsi, h_N, eta_N, delta_op, theta, h_P, W_P
		out.data[soln - 1].R_LOI = R_N;
		out.data[soln - 1].V_LOI = V_N;
		out.data[soln - 1].GMT_LOI = T_N;
		out.data[soln - 1].dv_LOI = DV;
		out.data[soln - 1].dgamma_LOI = dgamma;
		out.data[soln - 1].dpsi_LOI = dpsi;
		out.data[soln - 1].display.H_ND = h_N;
		out.data[soln - 1].display.eta_N = eta_N;
		out.data[soln - 1].display.delta_op = delta_op;
		out.data[soln - 1].display.theta = theta;
		out.data[soln - 1].display.h_P = R_P - opt.R_LLS;
		out.data[soln - 1].display.W_P = W_P;

		out.data[soln - 1].USSAV = U_S;
	}

	void LOITargeting::MINTHT(VECTOR3 U_DS)
	{
		VECTOR3 R_N, V_N, R_apo, U_P, U_S, NU, Rtemp, Vtemp;
		double dt, r, dv, cos_dpsi_Na, SGN, cos_a, dv2, cos_dpsi_N, theta, dpsi_Na;
		bool firstpass, plussoln;
		double ddt = 10.0;

		//Initialize +/- Min Theta solns with +/- coplanar soln
		out.data[4] = out.data[2];
		out.data[5] = out.data[3];

		//Propagate on the hyperbola conic to pre-perilune
		r = RA_LPO;
		dt = -OrbMech::time_radius(opt.SPH.R, -opt.SPH.V, r, 1.0, OrbMech::mu_Moon);
		OrbMech::rv_from_r0v0(opt.SPH.R, opt.SPH.V, dt, R_N, V_N, OrbMech::mu_Moon);
		firstpass = true;
	LOI_MINTHT_S:
		if (firstpass == false)
		{
			if (r < RP_LPO)
			{
				r = RP_LPO;
				dt = OrbMech::time_radius(opt.SPH.R, opt.SPH.V, r, 1.0, OrbMech::mu_Moon);
				OrbMech::rv_from_r0v0(opt.SPH.R, opt.SPH.V, dt, R_N, V_N, OrbMech::mu_Moon);
			}
		}
		plussoln = true;
	LOI_MINTHT_T:
		if (plussoln)
		{
			dv = opt.DV_maxp;
			SGN = 1.0;
		}
		else
		{
			dv = opt.DV_maxm;
			SGN = -1.0;
		}
		cos_dpsi_Na = DVDA(R_N, V_N, SGN, false, 0.0, dv);
		R_apo = unit(R_N);
		U_P = unit(U_DS - R_apo * dotp(U_DS, R_apo));
		cos_a = dotp(U_P, U_H);
		if (cos_dpsi_Na > 1.0)
		{
			goto LOI_MINTHT_B2;
		}
		if (cos_a >= cos_dpsi_Na)
		{
			cos_dpsi_N = cos_a;
			U_S = U_P;
			dv2 = DVDA(R_N, V_N, SGN, true, cos_dpsi_N, 0.0);
		}
		else
		{
			dv2 = dv;
			NU = unit(crossp(crossp(U_H, U_P), U_H));
			dpsi_Na = acos(cos_dpsi_Na);
			U_S = U_H * cos_dpsi_Na + NU * sin(dpsi_Na);
		}
		theta = acos(dotp(U_S, U_DS));
		if (theta < out.data[plussoln ? 4 : 5].display.theta)
		{
			CANS(R_N, V_N, opt.SPH.GMT + dt, U_S, RP_LPO, theta, SGN, plussoln ? 5 : 6);
		}
	LOI_MINTHT_B2:
		if (plussoln)
		{
			plussoln = false;
			goto LOI_MINTHT_T;
		}
		//Propagate through a dbeta to next hyp state => R
		OrbMech::rv_from_r0v0(R_N, V_N, ddt, Rtemp, Vtemp, OrbMech::mu_Moon);
		R_N = Rtemp;
		V_N = Vtemp;
		dt += ddt;
		r = length(R_N);
		if (r <= RA_LPO)
		{
			firstpass = false;
			goto LOI_MINTHT_S;
		}
	}

	void LOITargeting::INTER(VECTOR3 R_N, VECTOR3 V_N, double T_N, VECTOR3 U_L, VECTOR3 U_S, bool soln)
	{
		VECTOR3 R_N_u;
		double r_N, dw_a, R_p, dw, dh, SGN;

		if (soln)
		{
			SGN = 1.0;
		}
		else
		{
			SGN = -1.0;
		}

		r_N = length(R_N);
		R_N_u = unit(R_N);
		//Angle between position at node and desired post LOI2 perilune vector
		dw_a = acos(dotp(R_N_u, U_L));
		R_p = r_N;
		dw = dw_a + OrbMech::sign(dotp(crossp(U_L, R_N_u), U_S))*DWPSAV;
		dh = ALSAV * (1.0 - ELSAV * ELSAV) / (1.0 + ELSAV * cos(dw)) - R_p - DHPSAV;
		if (dh >= opt.dh_bias)
		{
			goto LOI_INTER_B3;
		}
		double A[5], R[4];
		double R_a, a, c1, K1, eca;
		int N, M;

		R_a = RA_LPO;
		dh = DHPSAV + opt.dh_bias;
		a = dw_a + OrbMech::sign(dotp(crossp(U_L, R_N_u), U_S))*DWPSAV;
		c1 = ALSAV * (1.0 - ELSAV * ELSAV) - dh;
		K1 = 2.0*R_a - r_N;
		eca = ELSAV * cos(a);

		//Scale to Earth radii
		r_N = r_N / OrbMech::R_Earth;
		K1 = K1 / OrbMech::R_Earth;
		dh = dh / OrbMech::R_Earth;
		R_a = R_a / OrbMech::R_Earth;
		c1 = c1 / OrbMech::R_Earth;

		A[0] = r_N * r_N*(1.0 + eca * eca) + ELSAV * ELSAV*(K1*K1 - r_N * r_N) - 2.0*K1*r_N*eca;
		A[1] = 2.0*r_N*r_N*(eca*eca*(dh - R_a) - R_a - c1) + 2.0*ELSAV*ELSAV*(K1*K1*dh - K1 * r_N*R_a - r_N * r_N*(dh - R_a)) + 2.0*eca*r_N*(K1*(R_a - dh + c1) + R_a * r_N);
		A[2] = r_N * r_N*(R_a*R_a + c1 * c1 + 4.0*c1*R_a + eca * eca*(R_a*R_a + dh * dh - 4.0*R_a*dh)) + ELSAV * ELSAV*(dh*dh*K1*K1 - 4.0*K1*R_a*r_N*dh + r_N * r_N*(4.0*R_a*dh - dh * dh))
			- 2.0*eca*(R_a*r_N*(K1*c1 - K1 * dh - r_N * dh + r_N * R_a) + r_N * r_N*c1*R_a - c1 * dh*r_N*K1);
		A[3] = 2.0*ELSAV*ELSAV*dh*dh*R_a*r_N*(r_N - K1) - 2.0*r_N*r_N*R_a*(c1*c1 + eca * eca*(dh*dh - R_a * dh) + c1 * R_a) + 2.0*eca*(r_N*r_N*R_a*R_a*(c1 - dh) - c1 * dh*R_a*r_N*(K1 + r_N));
		A[4] = r_N * r_N*R_a*R_a*(c1*c1 + 2.0*eca*c1*dh + eca * eca*dh*dh);

		OrbMech::SolveQuartic(A, R, N);

		//Scale back
		r_N = r_N * OrbMech::R_Earth;
		K1 = K1 * OrbMech::R_Earth;
		dh = dh * OrbMech::R_Earth;
		R_a = R_a * OrbMech::R_Earth;
		c1 = c1 * OrbMech::R_Earth;

		if (N == 0)
		{
			//Error
			return;
		}
		M = N;
		R_p = R[M - 1] * OrbMech::R_Earth;
	LOI_INTER_B2:
		dh = DELTAH(R_p, r_N, dw_a, U_L, R_N_u, U_S, SGN);
		if (abs(dh - opt.dh_bias) < 0.1)
		{
			goto LOI_INTER_B3;
		}
		if (M > 1)
		{
			M--;
			R_p = R[M - 1] * OrbMech::R_Earth;
			goto LOI_INTER_B2;
		}
		//Average each root pair and compute dh with DELTAH
		R_p = (R[0] + R[1]) / 2.0;
		dh = DELTAH(R_p, r_N, dw_a, U_L, R_N_u, U_S, SGN);
		if (abs(dh - opt.dh_bias) < 0.1)
		{
			goto LOI_INTER_B3;
		}
		if (N < 4)
		{
			goto LOI_INTER_B3;
		}
		R_p = (R[2] + R[3]) / 2.0;
		double dh2 = DELTAH(R_p, r_N, dw_a, U_L, R_N_u, U_S, SGN);
		if (abs(dh2 - opt.dh_bias) < 0.1)
		{
			goto LOI_INTER_B3;
		}
		//Use the R_p with the dh nearest to dhbias as the solution. Write online message and dh for this Rp
		if (abs(dh2 - opt.dh_bias) > abs(dh - opt.dh_bias))
		{
			R_p = (R[0] + R[1]) / 2.0;
		}
	LOI_INTER_B3:
		//DVDA(R_N, V_N, soln, 1.0, cos_dpsi_N, 0.0);
		CANS(R_N, V_N, T_N, U_S, R_p, 0.0, SGN, soln ? 1 : 2);
	}

	double LOITargeting::DELTAH(double R_p, double r_N, double dw_a, VECTOR3 U_L, VECTOR3 R_N_u, VECTOR3 U_S, double SGN)
	{
		double a, e, p, cos_wp, dw, R_NL, w_p;

		a = (RA_LPO + R_p) / 2.0;
		e = RA_LPO / a - 1.0;
		p = a * (1.0 - e * e);
		cos_wp = (p - r_N) / (r_N*e);
		if (cos_wp > 1.0)
		{
			cos_wp = 1.0;
		}
		w_p = acos(cos_wp);
		dw = dw_a + (OrbMech::sign(dotp(crossp(U_L, R_N_u), U_S)))*(DWPSAV + SGN * w_p);
		R_NL = ALSAV * (1.0 - ELSAV * ELSAV) / (1.0 + ELSAV * cos(dw));
		return R_NL - R_p - DHPSAV;
	}

	double LOITargeting::DELV2(VECTOR3 R_N, VECTOR3 U_S, double R_a, double R_p, double W_P)
	{
		VECTOR3 U_P_apo, R_N_apo, U_P, U_P1, U_P2;
		double a, e, a1, a2, e1, e2, P1, P2, cos_dw, sin_dw, dw, K_P, K_C, K_S, b, c, disc, SGN1, cos_eta2, eta2, eta1, R1, R2, V1, V2, gamma1, gamma2, cos_gamma1, cos_gamma2, dv, dgamma, dvtemp;
		bool first_pass;

		R_N_apo = unit(R_N);
		U_P_apo = R_N_apo * cos(W_P) - unit(crossp(U_S, R_N_apo))*sin(W_P);

		U_P = U_P_apo * cos(DWPSAV) - unit(crossp(U_P_apo, U_S))*sin(DWPSAV);
		a = (R_a + R_p) / 2.0;
		e = R_a / a - 1.0;

		if (e > ELSAV)
		{
			e1 = ELSAV;
			a1 = ALSAV;
			U_P1 = UPLSAV;
			e2 = e;
			a2 = a;
			U_P2 = U_P;
		}
		else
		{
			e1 = e;
			a1 = a;
			U_P1 = U_P;
			e2 = ELSAV;
			a2 = ALSAV;
			U_P2 = UPLSAV;
		}
		P2 = a2 * (1.0 - e2 * e2);
		P1 = a1 * (1.0 - e1 * e1);
		cos_dw = dotp(U_P1, U_P2);
		if (abs(cos_dw) > 1.0)
		{
			cos_dw = cos_dw / abs(cos_dw);
		}
		sin_dw = sqrt(1.0 - cos_dw * cos_dw);
		dw = OrbMech::sign(dotp(crossp(U_P1, U_P2), U_S))*acos(cos_dw);
		K_P = P1 - P2;
		K_C = P1 * e2 - cos_dw * P2*e1;
		K_S = P2 * e1*sin_dw;
		a = K_C * K_C + K_S * K_S;
		b = 2.0*K_P*K_C;
		c = K_P * K_P - K_S * K_S;
		disc = b * b - 4.0*a*c;
		if (disc < 0)
		{
			disc = 0.0;
		}
		SGN1 = 1.0;
	LOI_DELV2_B2:
		cos_eta2 = (-b + SGN1 * sqrt(disc)) / (2.0*a);
		if (abs(cos_eta2) > 1.0)
		{
			cos_eta2 = cos_eta2 / abs(cos_eta2);
		}
		eta2 = acos(cos_eta2);
		first_pass = true;
	LOI_DELV2_A3:
		eta1 = dw + eta2;
		R1 = P1 / (1.0 + e1 * cos(eta1));
		R2 = P2 / (1.0 + e2 * cos(eta2));
		if (abs(R1 - R2) < opt.RARPGT)
		{
			if (abs(eta1) > PI)
			{
				eta1 = eta1 - OrbMech::sign(eta1)*PI2;
			}
			goto LOI_DELV2_A4;
		}
		if (first_pass)
		{
			eta2 = -eta2;
			first_pass = false;
			goto LOI_DELV2_A3;
		}
		dv = 0.0;
		goto LOI_DELV2_B4;
	LOI_DELV2_A4:
		V1 = sqrt(OrbMech::mu_Moon*(2.0 / R1 - 1.0 / a1));
		V2 = sqrt(OrbMech::mu_Moon*(2.0 / R2 - 1.0 / a2));
		cos_gamma1 = sqrt(OrbMech::mu_Moon*P1) / R1 / V1;
		cos_gamma2 = sqrt(OrbMech::mu_Moon*P2) / R2 / V2;
		if (abs(cos_gamma1) > 1.0)
		{
			cos_gamma1 = cos_gamma1 / abs(cos_gamma1);
		}
		if (abs(cos_gamma2) > 1.0)
		{
			cos_gamma2 = cos_gamma2 / abs(cos_gamma2);
		}
		gamma1 = OrbMech::sign(eta1)*acos(cos_gamma1);
		gamma2 = OrbMech::sign(eta2)*acos(cos_gamma2);
		dgamma = abs(gamma1 - gamma2);
		dv = sqrt(V1*V1 + V2 * V2 - 2.0*V1*V2*cos(dgamma));
	LOI_DELV2_B4:
		if (SGN1 == 1.0)
		{
			SGN1 = -1.0;
			dvtemp = dv;
			goto LOI_DELV2_B2;
		}
		if (dv < dvtemp)
		{
			//Current solution
			return dv;
		}
		else
		{
			//Previous solution
			return dvtemp;
		}
	}

}