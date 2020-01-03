/****************************************************************************
This file is part of Project Apollo - NASSP

Translunar Midcourse Correction Processor

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

#include "OrbMech.h"
#include "TLMCC.h"

namespace GenIterator
{
	double CalcCost(const std::vector<double> &A, const std::vector<double> &B)
	{
		unsigned i;
		double D = 0.0;

		for (i = 0;i < A.size();i++)
		{
			D += A[i] * B[i] * B[i];
		}

		return D;
	}

	void tmat_mul_mat(double **A, double **B, int m, int n, int p, double **C)
	{
		//A is NxM
		//B is NxP
		//C is MxP

		int i, j, k;

		for (i = 0;i < m;i++)
		{
			for (j = 0;j < p;j++)
			{
				C[i][j] = 0.0;
				for (k = 0;k < n;k++)
				{
					C[i][j] += A[k][i] * B[k][j];
				}
			}
		}
	}

	void diag_mul_mat(const std::vector<double> &a, double **B, int m, int n, double **C)
	{
		//a is M
		//B is MxN
		//C is MxN

		int i, j;

		for (i = 0;i < m;i++)
		{
			for (j = 0;j < n;j++)
			{
				C[i][j] = a[i] * B[i][j];
			}
		}
	}

	void vec_mul_vec(const std::vector<double> &a, const std::vector<double> &b, double *c)
	{
		for (unsigned i = 0;i < a.size();i++)
		{
			c[i] = a[i] * b[i];
		}
	}

	void vec_mul_skal(const std::vector<double> &a, double b, double *c)
	{
		for (unsigned i = 0;i < a.size();i++)
		{
			c[i] = a[i] * b;
		}
	}

	void mat_mul_vec(double **A, double *b, int m, int n, double *c)
	{
		for (int i = 0;i < m;i++)
		{
			c[i] = 0.0;
			for (int j = 0;j < n;j++)
			{
				c[i] += A[i][j] * b[j];
			}
		}
	}

	void tmat_mul_vec(double **A, double *b, int m, int n, double *c)
	{
		for (int i = 0;i < m;i++)
		{
			c[i] = 0.0;
			for (int j = 0;j < n;j++)
			{
				c[i] += A[j][i] * b[j];
			}
		}
	}

	void mat_plus_vec(double **A, double *b, int n, double **C)
	{
		int i, j;

		for (i = 0;i < n;i++)
		{
			for (j = 0;j < n;j++)
			{
				C[i][j] = A[i][j];
				if (i == j)
				{
					C[i][j] += b[i];
				}
			}
		}
	}

	void CalcDX2(double **P, const std::vector<double> &W_X, const std::vector<double> &W_Y, double lambda, const std::vector<double> &dy, int m, int n, std::vector<double> &dx)
	{
		//W_X is M
		//W_Y is N
		//P is NxM
		//dy, b are N
		//c is M
		//A is M
		//B is NxM
		//C is MxM
		//D is MxM

		int *PP = new int[m + 1];
		double *b = new double[n];
		double *c = new double[m];
		double *A = new double[m];
		double **B = new double*[n];
		double **C = new double*[m];
		double **D = new double*[m];
		int i;

		for (i = 0;i < m;i++)
		{
			C[i] = new double[m];
			D[i] = new double[m];
		}
		for (i = 0;i < n;i++)
		{
			B[i] = new double[m];
		}

		vec_mul_vec(W_Y, dy, b);
		tmat_mul_vec(P, b, m, n, c);
		vec_mul_skal(W_X, lambda, A);
		diag_mul_mat(W_Y, P, n, m, B);
		tmat_mul_mat(P, B, m, n, m, C);
		mat_plus_vec(C, A, m, D);

		OrbMech::LUPDecompose(D, m, 0.0, PP);
		OrbMech::LUPSolve(D, PP, c, m, dx);

		delete[] b;
		delete[] c;
		delete[] A;
		delete[] B;
		delete[] C;
		delete[] D;
		delete[] PP;
	}

	void GeneralizedIterator(bool(*state_evaluation)(void*, std::vector<double>, void*, std::vector<double>&), const std::vector<double> &Y_min, const std::vector<double> &Y_max, const std::vector<double> &var_guess, const std::vector<double> &stepsizes, const std::vector<double> &x_weights, void *constants, void *data, const std::vector<int> &class_des, const std::vector<double> &y_weight, std::vector<double> &x_res, std::vector<double> &y_res)
	{
		double lambda, R, R_old, lambda1, w_avg;
		bool terminate, select = true, terminate2, hasclass3, sizing;
		int n, nMax, class1num;
		unsigned N, M, i, j;
		std::vector<double> Target, var_star, *v_l, *Y, var_star_temp, Y_star, C, dx, dy, dy_temp, W_Y, W_Y_apo, W_X;

		//Get number of independent and dependent variables
		M = var_guess.size();
		N = Y_min.size();
		//Set up some vectors
		v_l = new std::vector<double>[M];
		Y = new std::vector<double>[M];

		var_star.assign(M, 0);
		var_star_temp.assign(M, 0);
		dx.assign(M, 0);
		W_X.assign(M, 0);

		Target.assign(N, 0);
		Y_star.assign(N, 0);
		C.assign(N, 0);
		dy.assign(N, 0);
		dy_temp.assign(N, 0);
		W_Y.assign(N, 0);
		W_Y_apo.assign(N, 0);

		double **P = new double*[N];

		for (i = 0;i < M;i++)
		{
			v_l[i].assign(M, 0);
			Y[i].assign(N, 0);
		}

		for (i = 0;i < N;i++)
		{
			P[i] = new double[M];
		}

		//Set up iteration counters
		nMax = 100;
		n = 0;

		//Calculate target as the average between min and max value
		for (i = 0;i < N;i++)
		{
			Target[i] = (Y_max[i] + Y_min[i]) / 2.0;
		}
		//Set up initial guess
		for (i = 0;i < M;i++)
		{
			var_star[i] = var_guess[i];
		}

		//Use initial guess to get a first vector
		state_evaluation(data, var_star, constants, Y_star);
		for (i = 0;i < N;i++)
		{
			dy[i] = Target[i] - Y_star[i];
		}

		//Initial guess
		lambda = pow(2, -28);
		//This needs work
		for (i = 0;i < M;i++)
		{
			W_X[i] = x_weights[i];
		}
		//Select y weights based on class designation
		class1num = 0;
		hasclass3 = false;
		w_avg = 1.0;
		for (i = 0;i < N;i++)
		{
			if (class_des[i] == 1)
			{
				W_Y[i] = pow(2, -40) / pow((Y_max[i] - Y_min[i]) / 2.0, 2);
				class1num++;
				w_avg *= W_Y[i];
			}
			else if (class_des[i] == 3)
			{
				if (hasclass3)
				{
					//Error
				}
				else
				{
					hasclass3 = true;
				}
			}
		}
		w_avg = pow(w_avg, 1.0 / ((double)(class1num)));
		for (i = 0;i < N;i++)
		{
			if (class_des[i] == 2)
			{
				W_Y[i] = y_weight[i] * w_avg;
			}
			else if (class_des[i] == 3)
			{
				W_Y[i] = y_weight[i] * pow(10, -4)*pow(2, -40)*((double)(class1num)) / pow(dy[i], 2);
			}
		}

		do
		{
			terminate = true;

			//Check on the class variables
			for (i = 0;i < N;i++)
			{
				if (class_des[i] == 1)
				{
					C[i] = 1.0;
					if ((Y_star[i] <= Y_min[i]) || (Y_star[i] >= Y_max[i]))
					{
						terminate = false;
					}
				}
				else if (class_des[i] == 2)
				{
					if ((Y_star[i] > Y_min[i]) && (Y_star[i] < Y_max[i]))
					{
						C[i] = 0.0;
					}
					else
					{
						C[i] = 1.0;
						terminate = false;
					}
				}
				else
				{
					C[i] = 1.0;
					if (terminate)
					{
						if (select)
						{
							W_Y[i] = y_weight[i] * pow(2, -32)*((double)(class1num)) / pow(dy[i], 2);
						}
						select = false;
					}
					/*else
					{
						if (select == false)
						{
							W_Y[i] = y_weight[i] * pow(10, -4)*pow(2, -40)*((double)(class1num)) / pow(dy[i], 2);
							select = true;
						}
					}*/
					terminate = false;
				}
				W_Y_apo[i] = W_Y[i] * C[i];
			}

			if (terminate)
			{
				break;
			}

			if (n == 0)
			{
				R_old = CalcCost(W_Y_apo, dy);
			}

			//x-vectors for partial derivatives
			for (i = 0;i < M;i++)
			{
				v_l[i] = var_star;
				v_l[i][i] += stepsizes[i];
			}

			//Evaluate these vectors
			for (i = 0;i < M;i++)
			{
				state_evaluation(data, v_l[i], constants, Y[i]);
			}
			//Calculate partial derivatives matrix (Jacobi)
			for (i = 0;i < N;i++)
			{
				for (j = 0;j < M;j++)
				{
					P[i][j] = (Y[j][i] - Y_star[i]) / stepsizes[j];
				}
			}
			//Inhibitor control
			if (lambda > pow(2, -49))
			{
				if (select)
				{
					lambda = lambda / 64.0;
				}
				else
				{
					lambda = lambda / 2.0;
				}
			}
			do
			{
				CalcDX2(P, W_X, W_Y_apo, lambda, dy, M, N, dx);
				sizing = false;
				for (i = 0;i < M;i++)
				{
					if (abs(dx[i]) > 65536.0*stepsizes[i])
					{
						sizing = true;
						break;
					}
				}

				if (sizing == false)
				{
					lambda1 = lambda;
					break;
				}
				lambda = lambda * 8.0;

			} while (sizing);
			do
			{
				CalcDX2(P, W_X, W_Y_apo, lambda1, dy, M, N, dx);
				for (i = 0;i < M;i++)
				{
					var_star_temp[i] = var_star[i] + dx[i];
				}
				state_evaluation(data, var_star_temp, constants, Y_star);

				for (i = 0;i < N;i++)
				{
					dy_temp[i] = Target[i] - Y_star[i];

					if (class_des[i] == 2)
					{
						if ((Y_star[i] > Y_min[i]) && (Y_star[i] < Y_max[i]))
						{
							C[i] = 0.0;
						}
						else
						{
							C[i] = 1.0;
						}
						W_Y_apo[i] = W_Y[i] * C[i];
					}
				}

				R = CalcCost(W_Y_apo, dy_temp);

				if (R <= R_old)
				{
					lambda = lambda1;
					break;
				}
				else
				{
					lambda1 = 8.0 * lambda1;
				}
			} while (R > R_old);

			terminate2 = true;

			for (i = 0;i < M;i++)
			{
				if (abs(dx[i]) > stepsizes[i] / 100.0)
				{
					terminate2 = false;
				}
			}

			if (terminate2)
			{
				break;
			}

			R_old = R;
			CalcDX2(P, W_X, W_Y_apo, lambda, dy, M, N, dx);
			for (i = 0;i < M;i++)
			{
				var_star[i] += dx[i];
			}
			state_evaluation(data, var_star, constants, Y_star);
			for (i = 0;i < N;i++)
			{
				dy[i] = Target[i] - Y_star[i];
			}
			n++;
		} while (nMax >= n);

		delete[] v_l;
		delete[] Y;
		delete[] P;

		x_res = var_star;
		y_res = Y_star;
	}
}

TLMCCProcessor::TLMCCProcessor()
{
	R_E = OrbMech::R_Earth;
	R_M = OrbMech::R_Moon;
	mu_E = OrbMech::mu_Earth;
	mu_M = OrbMech::mu_Moon;
	gamma_reentry = -6.52*RAD;
	isp_SPS = 3080.0;
	isp_DPS = 3107.0;
}

void TLMCCProcessor::Init(PZEFEM *ephem, TLMCCDataTable data, TLMCCMEDQuantities med, TLMCCMissionConstants cst)
{
	ephemeris = ephem;
	
	hMoon = oapiGetObjectByName("Moon");

	DataTable = data;
	MEDQuantities = med;
	Constants = cst;
}

void TLMCCProcessor::Main()
{
	sv_MCC = OrbMech::coast(MEDQuantities.sv0, MEDQuantities.T_MCC - OrbMech::GETfromMJD(MEDQuantities.sv0.MJD, MEDQuantities.GETBase));
	if (sv_MCC.gravref == oapiGetObjectByName("Earth"))
	{
		KREF_MCC = 1;
	}
	else
	{
		KREF_MCC = 2;
	}
	if (!MEDQuantities.Config || MEDQuantities.useSPS)
	{
		isp_MCC = isp_SPS;
	}
	else
	{
		isp_MCC = isp_DPS;
	}

	switch (MEDQuantities.Mode)
	{
	case 1:
		Option1();
		break;
	case 2:
		Option2();
		break;
	case 3:
		Option3();
		break;
	case 4:
		Option4();
		break;
	case 5:
		Option5();
		break;
	case 6:
		Option6();
		break;
	case 7:
		Option7();
		break;
	case 8:
		Option8();
		break;
	case 9:
		Option9A();
		break;
	case 10:
		Option9B();
		break;
	}

	//TBD: Calc display quantities
}

void TLMCCProcessor::Option1()
{
	//Step 1
	VECTOR3 guess;
	VECTOR3 DV1 = ConicTLMC(DataTable.MJD_pc2, 0.0, DataTable.H_pc2 + DataTable.R_lls, DataTable.lat_pc2, DataTable.lng_pc2, guess);
	//Step 2
	VECTOR3 DV2 = IntegratedTLMC(DataTable.MJD_pc2, DataTable.H_pc2 + DataTable.R_lls, DataTable.lat_pc2, guess);
	//Step 3
	VECTOR3 DV3 = IntegratedXYZTTrajectory(sv_MCC, DataTable.H_nd, DataTable.lat_nd, DataTable.lng_nd, DataTable.MJD_nd, DV2);
	VECTOR3 RF, VF;
	double mfm0;
	
	BURN(sv_MCC.R, sv_MCC.V, DV3.x, DV3.y, DV3.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;
}

void TLMCCProcessor::Option2()
{
	std::vector<double> BAP_guess3, BAP_guess4, BAP_res3, BAP_res4;

	//Step 1
	VECTOR3 guess;
	VECTOR3 DV1 = ConicTLMC(DataTable.MJD_pc2, 0.0, DataTable.H_pc2 + DataTable.R_lls, DataTable.lat_pc2, PI, guess);

	//Step 2
	VECTOR3 DV2 = ConicFreeReturnFlyby(sv_MCC, DataTable.H_pc2, DataTable.lat_pc2, DV1);

	//Step 3
	std::vector<double> BAP_guess2{ DV2.x,DV2.y,DV2.z, DataTable.gamma_loi, DataTable.dpsi_loi,DataTable.dt_lls };
	ConicFreeReturnFixedOrbitLOI2BAP(sv_MCC, BAP_guess2, BAP_guess3, BAP_res3);

	//Step 4
	ConicFreeReturnFixedOrbitLOI2BAP(sv_MCC, BAP_guess3, BAP_guess4, BAP_res4, true, mass_stored);

	//Step 5
	double h_pl4, MJD_pl4, lat_pl4;
	h_pl4 = h_pl_stored;
	MJD_pl4 = MJD_pl_stored;
	lat_pl4 = lat_pl_stored;
	VECTOR3 DV5 = IntegratedTLMC(MJD_pl4, h_pl4 + DataTable.R_lls, lat_pl4, guess);

	//Step 6
	VECTOR3 DV6 = IntegratedFreeReturnFlyby(sv_MCC, h_pl4, lat_pl4, DV5);

	VECTOR3 RF, VF;
	double mfm0;
	BURN(sv_MCC.R, sv_MCC.V, DV6.x, DV6.y, DV6.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;
}

void TLMCCProcessor::Option3()
{
	//Step 1
	VECTOR3 guess;
	VECTOR3 DV1 = ConicTLMC(DataTable.MJD_pc2, 0.0, DataTable.H_pc2 + DataTable.R_lls, DataTable.lat_pc2, DataTable.lng_pc2, guess);

	//Step 2
	VECTOR3 DV2 = ConicFreeReturnFlyby(sv_MCC, DataTable.H_pc2, DataTable.lat_pc2, DV1);
}

void TLMCCProcessor::Option4()
{

}

void TLMCCProcessor::Option5()
{

}

void TLMCCProcessor::Option6()
{
	//Step 1
	VECTOR3 guess;
	VECTOR3 DV1 = ConicTLMC(DataTable.MJD_pc1, 0.0, DataTable.H_pc1 + DataTable.R_lls, DataTable.lat_pc1, PI, guess);
	//Step 2
	VECTOR3 DV2 = IntegratedTLMC(DataTable.MJD_pc1, DataTable.H_pc1 + DataTable.R_lls, DataTable.lat_pc1, guess);
	//Step4
	VECTOR3 DV3 = IntegratedFreeReturnFlyby(sv_MCC, DataTable.H_pc1, DataTable.lat_pc1, DV2);
	VECTOR3 RF, VF;
	double mfm0;

	BURN(sv_MCC.R, sv_MCC.V, DV3.x, DV3.y, DV3.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;
}

void TLMCCProcessor::Option7()
{
	//Step 1
	VECTOR3 guess;
	VECTOR3 DV1 = ConicTLMC(DataTable.MJD_pc1, 0.0, MEDQuantities.H_pl + DataTable.R_lls, DataTable.lat_pc1, PI, guess);
	//Step 2
	VECTOR3 DV2 = IntegratedTLMC(DataTable.MJD_pc1, MEDQuantities.H_pl + DataTable.R_lls, DataTable.lat_pc1, guess);
	//Step4
	VECTOR3 DV3 = IntegratedFreeReturnFlyby(sv_MCC, MEDQuantities.H_pl, DataTable.lat_pc1, DV2);
	VECTOR3 RF, VF;
	double mfm0;

	BURN(sv_MCC.R, sv_MCC.V, DV3.x, DV3.y, DV3.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;
}

void TLMCCProcessor::Option8()
{
	//Step 1
	VECTOR3 guess;
	VECTOR3 DV1 = ConicTLMC(DataTable.MJD_pc1, 0.0, DataTable.H_pc1 + DataTable.R_lls, DataTable.lat_pc1, PI, guess);

	//Step 2
	VECTOR3 R_EM, V_EM, R_ES;
	double incl_pg, lat_split;
	EPHEM(DataTable.MJD_pc1, R_EM, V_EM, R_ES);
	R_EM = rhtmul(OrbMech::GetObliquityMatrix(BODY_EARTH, DataTable.MJD_pc1), R_EM);
	incl_pg = abs(asin(R_EM.z / length(R_EM))) + 2.0*RAD;
	VECTOR3 DV2 = ConicFreeReturnInclinationFlyby(sv_MCC, DataTable.H_pc1, incl_pg, DV1);
	lat_split = lat_pl_stored;
	if (MEDQuantities.INCL_fr > 0)
	{
		lat_split += 2.0*RAD;
	}
	else
	{
		lat_split -= 2.0*RAD;
	}

	//Step 3
	VECTOR3 DV3 = ConicFreeReturnFlyby(sv_MCC, DataTable.H_pc1, lat_split, DV1);

	VECTOR3 DV_TLMC = IntegratedTLMC(MJD_pl_stored, DataTable.H_pc1 + DataTable.R_lls, lat_split, guess);

	//Step 4
	VECTOR3 DV4 = IntegratedFreeReturnFlyby(sv_MCC, DataTable.H_pc1, lat_split, DV_TLMC);
	VECTOR3 RF, VF, DV_temp;
	double mfm0;
	BURN(sv_MCC.R, sv_MCC.V, DV4.x, DV4.y, DV4.z, isp_MCC, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;

	//Step 5
	VECTOR3 DV5 = ConicFreeReturnInclinationFlyby(sv_MCC, DataTable.H_pc1, inc_fr_stored, DV3);
	MPTSV S1 = sv_MCC;
	MPTSV S2C = sv_MCC;
	BURN(sv_MCC.R, sv_MCC.V, DV5.x, DV5.y, DV5.z, isp_MCC, mfm0, RF, VF);
	S2C.V = VF;
	MPTSV S_apo = sv_MCC;
	S_apo.V = S2C.V - DV_temp;
	double theta, phi, r, v1, v2, gamma1, gamma2, psi1, psi2;
	RVIO(true, S_apo.R, S_apo.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S2C.R, S2C.V, r, v2, theta, phi, gamma2, psi2);
	VECTOR3 NewGuess = _V(v2 - v1, gamma2 - gamma1, psi2 - psi1);

	double lat_pl_max, lat_pl_min;
	if (MEDQuantities.INCL_fr > 0)
	{
		lat_pl_max = lat_split + 20.0*RAD;
		lat_pl_min = lat_split - 0.5*RAD;
	}
	else
	{
		lat_pl_max = lat_split + 0.5*RAD;
		lat_pl_min = lat_split - 20.0*RAD;
	}

	//Step 6
	VECTOR3 DV6 = ConicFreeReturnInclinationFlyby(S_apo, DataTable.H_pc1, abs(MEDQuantities.INCL_fr), NewGuess, lat_pl_min, lat_pl_max);
	BURN(S_apo.R, S_apo.V, DV6.x, DV6.y, DV6.z, isp_MCC, mfm0, RF, VF);
	DV_temp = VF - S_apo.V;
	VF = sv_MCC.V + DV_temp;
	RVIO(true, sv_MCC.R, sv_MCC.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, RF, VF, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V(v2 - v1, gamma2 - gamma1, psi2 - psi1);

	//Step 7
	VECTOR3 DV7 = IntegratedFreeReturnInclinationFlyby(sv_MCC, DataTable.H_pc1, abs(MEDQuantities.INCL_fr), NewGuess);
	BURN(sv_MCC.R, sv_MCC.V, DV7.x, DV7.y, DV7.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;
}

void TLMCCProcessor::Option9A()
{
	bool recycle = false;

	//Step 1
	VECTOR3 guess;
	VECTOR3 DV1 = ConicTLMC(DataTable.MJD_pc1, 0.0, DataTable.H_pc1 + DataTable.R_lls, DataTable.lat_pc1, PI, guess);

	//Step 2
	VECTOR3 R_EM, V_EM, R_ES;
	double incl_pg, lat_split;
	EPHEM(DataTable.MJD_pc1, R_EM, V_EM, R_ES);
	R_EM = rhtmul(OrbMech::GetObliquityMatrix(BODY_EARTH, DataTable.MJD_pc1), R_EM);
	incl_pg = abs(asin(R_EM.z / length(R_EM))) + 2.0*RAD;
	VECTOR3 DV2 = ConicFreeReturnInclinationFlyby(sv_MCC, DataTable.H_pc1, incl_pg, DV1);
	lat_split = lat_pl_stored;
	lat_split += 2.0*RAD;

	//Step 3
	VECTOR3 DV3 = ConicFreeReturnFlyby(sv_MCC, DataTable.H_pc1, lat_split, DV1);

	VECTOR3 DV_TLMC = IntegratedTLMC(MJD_pl_stored, DataTable.H_pc1 + DataTable.R_lls, lat_split, guess);

	//Step 4
	VECTOR3 DV4 = IntegratedFreeReturnFlyby(sv_MCC, DataTable.H_pc1, lat_split, DV_TLMC);
	double inc_fr_conv = inc_fr_stored;
	VECTOR3 RF, VF, DV_temp;
	double mfm0;
	BURN(sv_MCC.R, sv_MCC.V, DV4.x, DV4.y, DV4.z, isp_MCC, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;

	//Step 5
	VECTOR3 DV5 = ConicFreeReturnInclinationFlyby(sv_MCC, DataTable.H_pc1, inc_fr_conv, DV3);
	MPTSV S1 = sv_MCC;
	MPTSV S2C = sv_MCC;
	BURN(sv_MCC.R, sv_MCC.V, DV5.x, DV5.y, DV5.z, isp_MCC, mfm0, RF, VF);
	S2C.V = VF;
	MPTSV S_apo;
	VECTOR3 NewGuess, DV6, DV7, DV8, DV9;
	double h_conv, theta, phi, r, v1, v2, gamma1, gamma2, psi1, psi2, inc_opt, v_c, dv_char8, dv_char9;

	h_conv = DataTable.H_pc1;

TLMCC_Option_9A_C:
	
	S_apo = sv_MCC;
	S_apo.V = S2C.V - DV_temp;
	
	RVIO(true, S_apo.R, S_apo.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S2C.R, S2C.V, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V(v2 - v1, gamma2 - gamma1, psi2 - psi1);

	if (recycle == false)
	{
		//Step 6
		DV6 = ConicFreeReturnInclinationFlyby(S_apo, h_conv, inc_fr_conv, NewGuess);
		//Step 7
		DV7 = ConicFreeReturnOptimizedInclinationFlyby(S_apo, inc_fr_conv - 0.01*RAD, inc_fr_conv + 0.01*RAD, 1, DV6);
		//BURN(S_apo.R, S_apo.V, 3, 0.0, 0.0, DV7.x, DV7.y, DV7.z, isp_MCC, 0.0, v_c, dv_char7, mfm0, RF, VF);
		NewGuess = DV7;
	}
	//Step 8
	DV8 = ConicFreeReturnOptimizedInclinationFlyby(S_apo, 0.0*RAD, 88.0*RAD, 2, NewGuess);
	inc_opt = inc_fr_stored;
	h_conv = h_pl_stored;
	BURN(S_apo.R, S_apo.V, 3, 0.0, 0.0, DV8.x, DV8.y, DV8.z, isp_MCC, 0.0, v_c, dv_char8, mfm0, RF, VF);
	S2C.V = VF;
	DV_temp = S2C.V - S_apo.V;
	RVIO(true, S_apo.R, S1.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S1.R, S1.V + DV_temp, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V(v2 - v1, gamma2 - gamma1, psi2 - psi1);

	//Step 9
	DV9 = IntegratedFreeReturnInclinationFlyby(sv_MCC, h_conv, inc_opt, NewGuess);
	BURN(sv_MCC.R, sv_MCC.V, 3, 0.0, 0.0, DV9.x, DV9.y, DV9.z, isp_MCC, 0.0, v_c, dv_char9, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;

	if (abs(dv_char9 - dv_char8) > 1.0*0.3048 && ((dv_char9 / dv_char8) > 1.03 || (dv_char9 / dv_char8) < 1.0 / 1.03))
	{
		recycle = true;
		goto TLMCC_Option_9A_C;
	}

	BURN(sv_MCC.R, sv_MCC.V, DV9.x, DV9.y, DV9.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;
}

void TLMCCProcessor::Option9B()
{
	bool recycle = false;

	//Step 1
	VECTOR3 guess;
	VECTOR3 DV1 = ConicTLMC(DataTable.MJD_pc1, 0.0, DataTable.H_pc1 + DataTable.R_lls, DataTable.lat_pc1, PI, guess);

	//Step 2
	VECTOR3 R_EM, V_EM, R_ES;
	double incl_pg, lat_split;
	EPHEM(DataTable.MJD_pc1, R_EM, V_EM, R_ES);
	R_EM = rhtmul(OrbMech::GetObliquityMatrix(BODY_EARTH, DataTable.MJD_pc1), R_EM);
	incl_pg = abs(asin(R_EM.z / length(R_EM))) + 2.0*RAD;
	VECTOR3 DV2 = ConicFreeReturnInclinationFlyby(sv_MCC, DataTable.H_pc1, incl_pg, DV1);
	lat_split = lat_pl_stored;
	if (MEDQuantities.INCL_fr > 0)
	{
		lat_split += 2.0*RAD;
	}
	else
	{
		lat_split -= 2.0*RAD;
	}

	//Step 3
	VECTOR3 DV3 = ConicFreeReturnFlyby(sv_MCC, DataTable.H_pc1, lat_split, DV1);

	VECTOR3 DV_TLMC = IntegratedTLMC(MJD_pl_stored, DataTable.H_pc1 + DataTable.R_lls, lat_split, guess);

	//Step 4
	VECTOR3 DV4 = IntegratedFreeReturnFlyby(sv_MCC, DataTable.H_pc1, lat_split, DV_TLMC);
	double inc_fr_conv = inc_fr_stored;
	VECTOR3 RF, VF, DV_temp;
	double mfm0;
	BURN(sv_MCC.R, sv_MCC.V, DV4.x, DV4.y, DV4.z, isp_MCC, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;

	//Step 5
	VECTOR3 DV5 = ConicFreeReturnInclinationFlyby(sv_MCC, DataTable.H_pc1, inc_fr_conv, DV3);
	MPTSV S1 = sv_MCC;
	MPTSV S2C = sv_MCC;
	MPTSV S_apo;
	VECTOR3 NewGuess, DV6, DV7, DV8;
	double theta, phi, r, v1, v2, gamma1, gamma2, psi1, psi2, lat_pl_max, lat_pl_min, v_c, dv_char7, h_conv, inc_conv, dv_char8;

	BURN(sv_MCC.R, sv_MCC.V, DV5.x, DV5.y, DV5.z, isp_MCC, mfm0, RF, VF);
	S2C.V = VF;

TLMCC_Option_9B_C:

	S_apo = sv_MCC;
	S_apo.V = S2C.V - DV_temp;

	
	RVIO(true, S_apo.R, S_apo.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S2C.R, S2C.V, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V(v2 - v1, gamma2 - gamma1, psi2 - psi1);
 
	if (MEDQuantities.INCL_fr > 0)
	{
		lat_pl_max = lat_split + 20.0*RAD;
		lat_pl_min = lat_split - 0.5*RAD;
	}
	else
	{
		lat_pl_max = lat_split + 0.5*RAD;
		lat_pl_min = lat_split - 20.0*RAD;
	}

	if (recycle == false)
	{
		//Step 6
		DV6 = ConicFreeReturnInclinationFlyby(S_apo, MEDQuantities.H_pl_min + 20.0*1852.0, abs(MEDQuantities.INCL_fr), NewGuess, lat_pl_min, lat_pl_max);
	}

	//Step 7
	DV7 = ConicFreeReturnOptimizedInclinationFlyby(S_apo, abs(MEDQuantities.INCL_fr) - 0.01*RAD, abs(MEDQuantities.INCL_fr) + 0.01*RAD, 1, DV6);
	inc_conv = inc_fr_stored;
	h_conv = h_pl_stored;
	BURN(S_apo.R, S_apo.V, 3, 0.0, 0.0, DV7.x, DV7.y, DV7.z, isp_MCC, 0.0, v_c, dv_char7, mfm0, RF, VF);
	S2C.V = VF;
	DV_temp = S2C.V - S_apo.V;
	RVIO(true, S_apo.R, S1.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, S1.R, S1.V + DV_temp, r, v2, theta, phi, gamma2, psi2);
	NewGuess = _V(v2 - v1, gamma2 - gamma1, psi2 - psi1);

	//Step 8
	DV8 = IntegratedFreeReturnInclinationFlyby(sv_MCC, h_conv, inc_conv, NewGuess);
	BURN(sv_MCC.R, sv_MCC.V, 3, 0.0, 0.0, DV8.x, DV8.y, DV8.z, isp_MCC, 0.0, v_c, dv_char8, mfm0, RF, VF);
	DV_temp = VF - sv_MCC.V;

	if (abs(dv_char8 - dv_char7) > 1.0*0.3048 && ((dv_char8 / dv_char7) > 1.03 || (dv_char8 / dv_char7) < 1.0 / 1.03))
	{
		recycle = true;
		goto TLMCC_Option_9B_C;
	}

	BURN(sv_MCC.R, sv_MCC.V, DV8.x, DV8.y, DV8.z, isp_MCC, mfm0, RF, VF);
	DV_MCC = VF - sv_MCC.V;
}

VECTOR3 TLMCCProcessor::ConicTLMC(double MJD_P, double dt, double r_pl, double lat, double lng, VECTOR3 &pl_state)
{
	double lambda_pl, v_pl;

	EmpiricalFirstGuess(r_pl, lng, dt, v_pl, lambda_pl);

	void *constPtr;
	TLMCCFirstGuessVars constants;

	constants.MJD_pl = MJD_P;
	constants.gamma_pl = 0.0;
	constants.lat_pl = lat;
	constants.r_pl = r_pl;
	constPtr = &constants;

	bool ConicTLMCPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>, void*, std::vector<double>& arr) = &ConicTLMCPointer;

	std::vector<double> Y_min, Y_max;

	Y_min.push_back(sv_MCC.R.x - 0.0657*1852.0);
	Y_min.push_back(sv_MCC.R.y - 0.0657*1852.0);
	Y_min.push_back(sv_MCC.R.z - 0.0657*1852.0);

	Y_max.push_back(sv_MCC.R.x + 0.0657*1852.0);
	Y_max.push_back(sv_MCC.R.y + 0.0657*1852.0);
	Y_max.push_back(sv_MCC.R.z + 0.0657*1852.0);

	std::vector<double> step { 0.01*0.3048, pow(2,-19), pow(2,-19) };
	std::vector<double> guess { v_pl, 270.0*RAD, lambda_pl };
	std::vector<double> x_weight { 0.3,512.0,512.0 };
	std::vector<double> y_weight { 0,0,0 };
	std::vector<int> class_des { 1,1,1 };

	if (KREF_MCC == 1)
	{
		Y_min.push_back(0.0);
		Y_max.push_back(92.0*RAD);
		y_weight.push_back(64);
		class_des.push_back(2);
	}

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, Y_min, Y_max, guess, step, x_weight, constPtr, (void*)this, class_des, y_weight, result, y_vals);
	pl_state = _V(result[0], result[1], result[2]);
	
	double r, v1, v2, theta, phi, gamma1, psi1, gamma2, psi2;
	RVIO(true, sv_MCC.R, sv_MCC.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, sv_MCC.R, V_MCC_stored, r, v2, theta, phi, gamma2, psi2);
	return _V(v2 - v1, gamma2 - gamma1, psi2 - psi1); 
}

VECTOR3 TLMCCProcessor::IntegratedTLMC(double MJD_P, double r_pl, double lat, VECTOR3 guess)
{
	void *constPtr;
	TLMCCFirstGuessVars constants;

	constants.MJD_pl = MJD_P;
	constants.gamma_pl = 0.0;
	constants.lat_pl = lat;
	constants.r_pl = r_pl;
	constPtr = &constants;

	bool IntegratedTLMCComputerPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>, void*, std::vector<double>& arr) = &IntegratedTLMCComputerPointer;

	std::vector<double> Y_min, Y_max;

	Y_min.push_back(sv_MCC.R.x - 0.0657*1852.0);
	Y_min.push_back(sv_MCC.R.y - 0.0657*1852.0);
	Y_min.push_back(sv_MCC.R.z - 0.0657*1852.0);

	Y_max.push_back(sv_MCC.R.x + 0.0657*1852.0);
	Y_max.push_back(sv_MCC.R.y + 0.0657*1852.0);
	Y_max.push_back(sv_MCC.R.z + 0.0657*1852.0);

	std::vector<double> step{ 0.01*0.3048, pow(2,-19), pow(2,-19) };
	std::vector<double> guess2{ guess.x,guess.y,guess.z };
	std::vector<double> x_weight{ 0.3,512.0,512.0 };
	std::vector<double> y_weight{ 0,0,0 };
	std::vector<int> class_des{ 1,1,1 };

	if (KREF_MCC == 1)
	{
		Y_min.push_back(0.0);
		Y_max.push_back(92.0*RAD);
		y_weight.push_back(64);
		class_des.push_back(2);
	}

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, Y_min, Y_max, guess2, step, x_weight, constPtr, (void*)this, class_des, y_weight, result, y_vals);
	VECTOR3 newguess = _V(result[0], result[1], result[2]);

	double r, v1, v2, theta, phi, gamma1, gamma2, psi1, psi2;
	RVIO(true, sv_MCC.R, sv_MCC.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, sv_MCC.R, V_MCC_stored, r, v2, theta, phi, gamma2, psi2);
	return _V(v2 - v1, gamma2 - gamma1, psi2 - psi1);
}

VECTOR3 TLMCCProcessor::IntegratedXYZTTrajectory(MPTSV sv0, double H_nd, double lat_nd, double lng_nd, double MJD_node, VECTOR3 guess)
{
	void *constPtr;
	TLMCCIntegratingTrajectoryVars constants;

	constants.dt_node = (MJD_node - sv_MCC.MJD)*24.0*3600.0;
	constants.sv0 = sv0;
	constants.NodeStopIndicator = true;
	constPtr = &constants;

	bool IntegratedNodeComputerPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>, void*, std::vector<double>& arr) = &IntegratedNodeComputerPointer;

	std::vector<double> Y_min, Y_max;

	Y_min.push_back(H_nd - 0.5*1852.0);
	Y_min.push_back(lat_nd - 0.01*RAD);
	Y_min.push_back(lng_nd - 0.01*RAD);
	Y_min.push_back(90.0*RAD);

	Y_max.push_back(H_nd + 0.5*1852.0);
	Y_max.push_back(lat_nd + 0.01*RAD);
	Y_max.push_back(lng_nd + 0.01*RAD);
	Y_max.push_back(182.0*RAD);

	std::vector<double> step{ 0.01*0.3048, pow(2,-19), pow(2,-19) };
	std::vector<double> guess2{ guess.x,guess.y,guess.z };
	std::vector<double> x_weight{ 0.3,512.0,512.0 };
	std::vector<double> y_weight{ 0,0,0,64 };
	std::vector<int> class_des{ 1,1,1,2 };

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, Y_min, Y_max, guess2, step, x_weight, constPtr, (void*)this, class_des, y_weight, result, y_vals);
	return _V(result[0], result[1], result[2]);
}

VECTOR3 TLMCCProcessor::ConicFreeReturnInclinationFlyby(MPTSV sv0, double H_pl, double inc_pg, VECTOR3 guess, double lat_pl_min, double lat_pl_max)
{
	void *constPtr;
	TLMCCConicTrajectoryVars constants;

	constants.sv0 = sv0;
	constants.FreeReturnIndicator = true;
	constants.FreeReturnOnlyIndicator = true;
	constants.FreeReturnInclinationIndicator = true;
	constants.OptimizeIndicator = false;
	constPtr = &constants;

	bool ConicMissionComputerPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

	std::vector<double> Y_min, Y_max;

	Y_min.push_back(H_pl - 0.5*1852.0);
	Y_min.push_back(90.0*RAD);
	Y_min.push_back(64.0965*1852.0);
	Y_min.push_back(inc_pg - 0.01*RAD);

	Y_max.push_back(H_pl + 0.5*1852.0);
	Y_max.push_back(182.0*RAD);
	Y_max.push_back(67.5665*1852.0);
	Y_max.push_back(inc_pg + 0.01*RAD);

	std::vector<double> step { 0.01*0.3048, 0.001*RAD, 0.001*RAD };
	std::vector<double> guess2{ guess.x,guess.y,guess.z };
	std::vector<double> x_weight{ 1e-3,1.0,1.0 };
	std::vector<double> y_weight{ 0,1,0,0 };
	std::vector<int> class_des{ 1,2,1,1 };

	if (lat_pl_min != 0.0)
	{
		Y_min.push_back(lat_pl_min);
		Y_max.push_back(lat_pl_max);
		y_weight.push_back(32.0);
		class_des.push_back(2);
	}

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, Y_min, Y_max, guess2, step, x_weight, constPtr, (void*)this, class_des, y_weight, result, y_vals);
	return _V(result[0], result[1], result[2]);
}

VECTOR3 TLMCCProcessor::ConicFreeReturnOptimizedInclinationFlyby(MPTSV sv0, double inc_pg_min, double inc_pg_max, int inc_class, VECTOR3 guess)
{
	void *constPtr;
	TLMCCConicTrajectoryVars constants;

	constants.sv0 = sv0;
	constants.FreeReturnIndicator = true;
	constants.FreeReturnOnlyIndicator = true;
	constants.FreeReturnInclinationIndicator = true;
	constants.OptimizeIndicator = true;
	constPtr = &constants;

	bool ConicMissionComputerPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

	std::vector<double> Y_min, Y_max;

	Y_min.push_back(MEDQuantities.H_pl_min);
	Y_min.push_back(90.0*RAD);
	Y_min.push_back(inc_pg_min);
	Y_min.push_back(64.0965*1852.0);
	Y_min.push_back(MEDQuantities.CSMMass + 5000.0*0.453);

	Y_max.push_back(MEDQuantities.H_pl_max);
	Y_max.push_back(182.0*RAD);
	Y_max.push_back(inc_pg_max);
	Y_max.push_back(67.5665*1852.0);
	Y_max.push_back(MEDQuantities.CSMMass + 5000.0*0.453);

	std::vector<double> step { 0.01*0.3048, 0.001*RAD, 0.001*RAD };
	std::vector<double> guess2{ guess.x,guess.y,guess.z };
	std::vector<double> x_weight{ 1e-3,1.0,1.0 };
	std::vector<double> y_weight{ 8.0,1.0,0,0,8.0 };
	std::vector<int> class_des{ 2,2,inc_class,1,3 };

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, Y_min, Y_max, guess2, step, x_weight, constPtr, (void*)this, class_des, y_weight, result, y_vals);
	return _V(result[0], result[1], result[2]);
}

VECTOR3 TLMCCProcessor::IntegratedFreeReturnFlyby(MPTSV sv0, double H_pl, double lat_pl, VECTOR3 guess)
{
	void *constPtr;
	TLMCCIntegratingTrajectoryVars constants;
	constants.NodeStopIndicator = false;
	constants.LunarFlybyIndicator = true;
	constants.FreeReturnInclinationIndicator = false;
	constants.sv0 = sv0;
	constPtr = &constants;

	bool IntegratedNodeComputerPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>, void*, std::vector<double>& arr) = &IntegratedNodeComputerPointer;

	std::vector<double> Y_min, Y_max;

	Y_min.push_back(H_pl - 0.5*1852.0);
	Y_min.push_back(90.0*RAD);
	Y_min.push_back(lat_pl - 0.01*RAD);
	Y_min.push_back(0.0);
	Y_min.push_back(64.0965*1852.0);

	Y_max.push_back(H_pl + 0.5*1852.0);
	Y_max.push_back(182.0*RAD);
	Y_max.push_back(lat_pl + 0.01*RAD);
	Y_max.push_back(90.0*RAD);
	Y_max.push_back(67.5665*1852.0);

	std::vector<double> step { 0.1*0.3048, 0.001*RAD, 0.001*RAD };
	std::vector<double> guess2{ guess.x,guess.y,guess.z };
	std::vector<double> x_weight{ 1e-3,1.0,1.0 };
	std::vector<double> y_weight{ 0,1,0,1,0 };
	std::vector<int> class_des{ 1,2,1,2,1 };

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, Y_min, Y_max, guess2, step, x_weight, constPtr, (void*)this, class_des, y_weight, result, y_vals);
	return _V(result[0], result[1], result[2]);
}

VECTOR3 TLMCCProcessor::ConicFreeReturnFlyby(MPTSV sv0, double H_pl, double lat_pl, VECTOR3 guess)
{
	void *constPtr;
	TLMCCConicTrajectoryVars constants;
	constants.sv0 = sv0;
	constants.FreeReturnIndicator = true;
	constants.FreeReturnOnlyIndicator = true;
	constants.FreeReturnInclinationIndicator = false;

	constPtr = &constants;

	bool ConicMissionComputerPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

	std::vector<double> Y_min, Y_max;

	Y_min.push_back(H_pl - 0.5*1852.0);
	Y_min.push_back(90.0*RAD);
	Y_min.push_back(lat_pl - 0.01*RAD);
	Y_min.push_back(0.0);
	Y_min.push_back(64.0965*1852.0);

	Y_max.push_back(H_pl + 0.5*1852.0);
	Y_max.push_back(182.0*RAD);
	Y_max.push_back(lat_pl + 0.01*RAD);
	Y_max.push_back(90.0*RAD);
	Y_max.push_back(67.5665*1852.0);

	std::vector<double> step { 0.01*0.3048, pow(2,-19), pow(2,-19) };
	std::vector<double> guess2{ guess.x,guess.y,guess.z };
	std::vector<double> x_weight{ 1e-3,512.0,512.0 };
	std::vector<double> y_weight{ 0,64.0,0,8.0,0 };
	std::vector<int> class_des{ 1,2,1,2,1 };

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, Y_min, Y_max, guess2, step, x_weight, constPtr, (void*)this, class_des, y_weight, result, y_vals);
	return _V(result[0], result[1], result[2]);
}

VECTOR3 TLMCCProcessor::IntegratedFreeReturnInclinationFlyby(MPTSV sv0, double H_pl, double inc_fr, VECTOR3 guess)
{
	void *constPtr;
	TLMCCIntegratingTrajectoryVars constants;
	constants.NodeStopIndicator = false;
	constants.LunarFlybyIndicator = true;
	constants.FreeReturnInclinationIndicator = true;
	constants.sv0 = sv0;
	constPtr = &constants;

	bool IntegratedNodeComputerPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>, void*, std::vector<double>& arr) = &IntegratedNodeComputerPointer;

	std::vector<double> Y_min, Y_max;

	Y_min.push_back(H_pl - 0.5*1852.0);
	Y_min.push_back(inc_fr - 0.01*RAD);
	Y_min.push_back(90.0*RAD);
	Y_min.push_back(64.0965*1852.0);

	Y_max.push_back(H_pl + 0.5*1852.0);
	Y_max.push_back(inc_fr + 0.01*RAD);
	Y_max.push_back(182.0*RAD);
	Y_max.push_back(67.5665*1852.0);

	std::vector<double> step { 0.1*0.3048, 0.001*RAD, 0.001*RAD };
	std::vector<double> guess2{ guess.x,guess.y,guess.z };
	std::vector<double> x_weight{ 1e-3,1.0,1.0 };
	std::vector<double> y_weight{ 0,0,1.0,0 };
	std::vector<int> class_des{ 1,1,2,1 };

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, Y_min, Y_max, guess2, step, x_weight, constPtr, (void*)this, class_des, y_weight, result, y_vals);
	return _V(result[0], result[1], result[2]);
}

VECTOR3 TLMCCProcessor::ConicFreeReturnOptimizedFixedOrbitBAP(MPTSV sv0, VECTOR3 guess)
{
	void *constPtr;
	TLMCCConicTrajectoryVars constants;
	constants.sv0 = sv0;
	constants.FreeReturnIndicator = true;
	constants.FreeReturnOnlyIndicator = false;
	constants.FreeReturnInclinationIndicator = false;
	constants.AfterLOIStopIndicator = true;
	constants.FirstSelect = true;
	constants.FixedOrbitIndicator = true;
	constants.HasLOI2Indicator = false;

	constPtr = &constants;

	bool ConicMissionComputerPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

	std::vector<double> Y_min, Y_max;

	Y_min.push_back(-0.01*RAD);
	Y_min.push_back(90.0*RAD);
	Y_min.push_back(-0.1*1852.0);
	Y_min.push_back(64.0965*1852.0);
	Y_min.push_back(0.0);
	Y_min.push_back(100000.0*0.453);

	Y_max.push_back(0.01*RAD);
	Y_max.push_back(182.0*RAD);
	Y_max.push_back(0.1*1852.0);
	Y_max.push_back(75.0*RAD);
	Y_max.push_back(67.5665*1852.0);
	Y_max.push_back(100000.0*0.453);

	std::vector<double> step{ 0.1*0.3048, 0.001*RAD, 0.001*RAD, 0.001*RAD };
	std::vector<double> guess2{ guess.x,guess.y,guess.z, DataTable.gamma_loi };
	std::vector<double> x_weight{ 1e-3,1.0,1.0,1.0 };
	std::vector<double> y_weight{ 0,64,0,0,64,8.0 };
	std::vector<int> class_des{ 1,2,1,1,2,3 };

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, Y_min, Y_max, guess2, step, x_weight, constPtr, (void*)this, class_des, y_weight, result, y_vals);
	return _V(result[0], result[1], result[2]);
}

void TLMCCProcessor::ConicFreeReturnFixedOrbitLOI2BAP(MPTSV sv0, std::vector<double> guess, std::vector<double> &x_res, std::vector<double> &y_res, bool optimize, double mass)
{
	void *constPtr;
	TLMCCConicTrajectoryVars constants;
	constants.sv0 = sv0;
	constants.FreeReturnIndicator = true;
	constants.FreeReturnOnlyIndicator = false;
	constants.FreeReturnInclinationIndicator = false;
	constants.FirstSelect = true;
	constants.FixedOrbitIndicator = true;
	constants.HasLOI2Indicator = true;

	constPtr = &constants;

	bool ConicMissionComputerPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr);
	bool(*fptr)(void *, std::vector<double>, void*, std::vector<double>& arr) = &ConicMissionComputerPointer;

	std::vector<double> Y_min, Y_max;

	Y_min.push_back(40.0*1852.0);
	Y_min.push_back(90.0*RAD);
	Y_min.push_back(MEDQuantities.H_P_LPO1 - 0.5*1852.0);
	Y_min.push_back(64.0965*1852.0);
	Y_min.push_back(0.0);
	Y_min.push_back(DataTable.lat_lls - 0.01*RAD);
	Y_min.push_back(DataTable.lng_lls - 0.01*RAD);
	Y_min.push_back(DataTable.psi_lls - 0.01*RAD);

	Y_max.push_back(100.0*1852.0);
	Y_max.push_back(182.0*RAD);
	Y_max.push_back(MEDQuantities.H_P_LPO1 + 0.5*1852.0);
	Y_max.push_back(67.5665*1852.0);
	Y_max.push_back(75.0*RAD);
	Y_max.push_back(DataTable.lat_lls + 0.01*RAD);
	Y_max.push_back(DataTable.lng_lls + 0.01*RAD);
	Y_max.push_back(DataTable.psi_lls + 0.01*RAD);

	std::vector<double> step{ 0.00277*0.3048, pow(2,-21), pow(2,-19), pow(2,-19), pow(2,-19), pow(2,-18) };
	std::vector<double> guess2 = guess;
	std::vector<double> x_weight{ 0.3,512.0,512.0,1.0,1.0,1e-3 };
	std::vector<double> y_weight{ 1.0, 64.0, 0.0, 0.0, 8.0, 0.0, 0.0, 0.0 };
	std::vector<int> class_des{ 2, 2, 1, 1, 2, 1, 1, 1 };

	if (optimize)
	{
		Y_min.push_back(mass + 5000.0*0.453);
		Y_max.push_back(mass + 5000.0*0.453);
		y_weight.push_back(pow(2,-4));
		class_des.push_back(3);
		constants.OptimizeIndicator = true;
	}

	std::vector<double> result;
	std::vector<double> y_vals;
	GenIterator::GeneralizedIterator(fptr, Y_min, Y_max, guess2, step, x_weight, constPtr, (void*)this, class_des, y_weight, result, y_vals);
	x_res = result;
	y_res = y_vals;
}

void TLMCCProcessor::EmpiricalFirstGuess(double r, double lng, double dt, double &V, double &lambda)
{
	if (MEDQuantities.Mode == 1)
	{
		lambda = lng;
		V = sqrt(0.184 + 0.553 / (r / R_E))*R_E / 3600.0 - 0.0022*dt*3600.0 + (lng - PI)*13.5*0.3048;
	}
	else if (MEDQuantities.Mode == 2 || MEDQuantities.Mode == 3)
	{
		lambda = PI;
		V = sqrt(0.184 + 0.553 / (r / R_E))*R_E / 3600.0;
	}
	else if (MEDQuantities.Mode == 4 || MEDQuantities.Mode == 5)
	{
		lambda = lng - 0.025*dt*3600.0;
		V = sqrt(0.184 + 0.553 / (r / R_E))*R_E / 3600.0 - 0.0022*dt*3600.0 + (lng - PI)*13.5*0.3048;
	}
	else
	{
		lambda = PI;
		V = sqrt(0.184 + 0.553 / ((r + 20.0*1852.0) / R_E))*R_E / 3600.0;
	}
}

bool ConicTLMCPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr)
{
	return ((TLMCCProcessor*)data)->FirstGuessTrajectoryComputer(var, varPtr, arr);
}

bool TLMCCProcessor::FirstGuessTrajectoryComputer(std::vector<double> var, void *varPtr, std::vector<double>& arr)
{
	TLMCCFirstGuessVars *vars;
	vars = static_cast<TLMCCFirstGuessVars*>(varPtr);
	VECTOR3 R, V, R_MCC, V_MCC;
	double MJD, v_pl, psi_pl, lng_pl;
	bool err;

	v_pl = var[0];
	psi_pl = var[1];
	lng_pl = var[2];

	MJD = vars->MJD_pl;
	RVIO(false, R, V, vars->r_pl, v_pl, lng_pl, vars->lat_pl, vars->gamma_pl, psi_pl);
	LIBRAT(R, V, MJD, 3);

	if (KREF_MCC == 1)
	{
		VECTOR3 R_patch, V_patch;
		R_patch = R;
		V_patch = V;
		PATCH(R_patch, V_patch, MJD, -1, 2);
		err = CTBODY(R_patch, V_patch, MJD, sv_MCC.MJD, 1, mu_E, R_MCC, V_MCC);
		if (err) return true;
	}
	else
	{
		err = CTBODY(R, V, MJD, sv_MCC.MJD, 2, mu_M, R_MCC, V_MCC);
		if (err) return true;
	}

	//Store for later use
	V_MCC_stored = V_MCC;

	arr[0] = R_MCC.x;
	arr[1] = R_MCC.y;
	arr[2] = R_MCC.z;

	if (arr.size() > 3)
	{
		double r, v, theta, phi, gamma, psi;
		RVIO(true, R_MCC, V_MCC, r, v, theta, phi, gamma, psi);

		arr[3] = gamma;
	}

	return false;
}

bool IntegratedTLMCComputerPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr)
{
	return ((TLMCCProcessor*)data)->IntegratedTLMCComputer(var, varPtr, arr);
}

bool TLMCCProcessor::IntegratedTLMCComputer(std::vector<double> var, void *varPtr, std::vector<double>& arr)
{
	TLMCCFirstGuessVars *vars;
	vars = static_cast<TLMCCFirstGuessVars*>(varPtr);
	VECTOR3 R, V, R_MCC, V_MCC;
	double MJD, v_pl, psi_pl, lng_pl;

	v_pl = var[0];
	psi_pl = var[1];
	lng_pl = var[2];

	MJD = vars->MJD_pl;
	RVIO(false, R, V, vars->r_pl, v_pl, lng_pl, vars->lat_pl, vars->gamma_pl, psi_pl);
	LIBRAT(R, V, MJD, 3);
	OrbMech::oneclickcoast(R, V, MJD, (sv_MCC.MJD - MJD)*24.0*3600.0, R_MCC, V_MCC, hMoon, sv_MCC.gravref);

	//Store for later use
	V_MCC_stored = V_MCC;

	arr[0] = R_MCC.x;
	arr[1] = R_MCC.y;
	arr[2] = R_MCC.z;

	if (arr.size() > 3)
	{
		double r, v, theta, phi, gamma, psi;
		RVIO(true, R_MCC, V_MCC, r, v, theta, phi, gamma, psi);

		arr[3] = gamma;
	}

	return false;
}

bool IntegratedNodeComputerPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr)
{
	return ((TLMCCProcessor*)data)->IntegratedNodeComputer(var, varPtr, arr);
}

bool TLMCCProcessor::IntegratedNodeComputer(std::vector<double> var, void *varPtr, std::vector<double>& arr)
{
	TLMCCIntegratingTrajectoryVars *vars;
	vars = static_cast<TLMCCIntegratingTrajectoryVars*>(varPtr);
	MPTSV sv0;
	VECTOR3 RF, VF, R_node, V_node, H_pg;
	double dv_mcc, dgamma_mcc, dpsi_mcc, mfm0, H_nd, lat_nd, lng_nd, MJD_node, H_fr_rtny, inc_pg;

	dv_mcc = var[0];
	dgamma_mcc = var[1];
	dpsi_mcc = var[2];
	sv0 = vars->sv0;

	BURN(sv0.R, sv0.V, dv_mcc, dgamma_mcc, dpsi_mcc, isp_SPS, mfm0, RF, VF);
	if (vars->NodeStopIndicator)
	{
		OrbMech::oneclickcoast(RF, VF, sv0.MJD, vars->dt_node, R_node, V_node, sv0.gravref, hMoon);
		MJD_node = sv0.MJD + vars->dt_node / 24.0 / 3600.0;
		LIBRAT(R_node, V_node, MJD_node, 4);
		H_nd = length(R_node) - DataTable.R_lls;
		OrbMech::latlong_from_r(R_node, lat_nd, lng_nd);
		if (lng_nd < 0) lng_nd += PI2;
		H_pg = crossp(R_node, V_node);
		inc_pg = acos(H_pg.z / length(H_pg));

		arr[0] = H_nd;
		arr[1] = lat_nd;
		arr[2] = lng_nd;
		arr[3] = inc_pg;

		return false;
	}
	else
	{
		MPTSV sv1;
		MATRIX3 Rot;
		VECTOR3 R_temp, V_temp, HH_pl, H_equ;
		double H_pl, lat_pl, lng_pl, inc_pl, inc_fr;

		sv1.R = RF;
		sv1.V = VF;
		sv1.MJD = sv0.MJD;
		sv1.gravref = sv0.gravref;
		MPTSV sv_pl = OrbMech::PMMCEN(sv1, 0.0, 100.0*3600.0, 2, 0.0, 1.0);
	
		R_temp = sv_pl.R;
		V_temp = sv_pl.V;
		LIBRAT(R_temp, V_temp, sv_pl.MJD, 4);
		H_pl = length(R_temp) - DataTable.R_lls;
		OrbMech::latlong_from_r(R_temp, lat_pl, lng_pl);
		HH_pl = crossp(R_temp, V_temp);
		inc_pl = acos(HH_pl.z / length(HH_pl));

		MPTSV sv_reentry = OrbMech::PMMCEN(sv_pl, 0.0, 100.0*3600.0, 2, sin(gamma_reentry), 1.0);
		H_fr_rtny = length(sv_reentry.R) - R_E;
		Rot = OrbMech::GetObliquityMatrix(BODY_EARTH, sv_reentry.MJD);
		R_temp = rhtmul(Rot, sv_reentry.R);
		V_temp = rhtmul(Rot, sv_reentry.V);
		H_equ = crossp(R_temp, V_temp);
		inc_fr = inc_fr_stored = acos(H_equ.z / length(H_equ));

		if (vars->LunarFlybyIndicator)
		{
			double lat, lng, dlng;
			RNTSIM(sv_reentry.R, sv_reentry.V, sv_reentry.MJD, Constants.lambda_IP, lat, lng, dlng);
		}

		if (vars->FreeReturnInclinationIndicator)
		{
			arr[0] = H_pl;
			arr[1] = inc_fr;
			arr[2] = inc_pl;
			arr[3] = H_fr_rtny;
			return false;
		}
		else
		{
			arr[0] = H_pl;
			arr[1] = inc_pl;
			arr[2] = lat_pl;
			arr[3] = inc_fr;
			arr[4] = H_fr_rtny;
			return false;
		}
	}
}

bool ConicMissionComputerPointer(void *data, std::vector<double> var, void *varPtr, std::vector<double>& arr)
{
	return ((TLMCCProcessor*)data)->ConicMissionComputer(var, varPtr, arr);
}

bool TLMCCProcessor::ConicMissionComputer(std::vector<double> var, void *varPtr, std::vector<double>& arr)
{
	TLMCCConicTrajectoryVars *vars;
	vars = static_cast<TLMCCConicTrajectoryVars*>(varPtr);
	MPTSV sv0, SGSLOI;
	VECTOR3 RF, VF, R_patch, V_patch, R_pl, V_pl, R_temp, V_temp, HH_pl, H;
	double dv_mcc, dgamma_mcc, dpsi_mcc, m_before_MCC, mfm0, beta, MJD_patch, MJD_pl, ainv, m_before_LOI, H_fr_rtny, DT_1st_pass;
	double H_pl, lat_pl, lng_pl, inc_pl, i_pg, gamma_loi, dpsi_loi, m_before_DOI, ainv_pl, a, e, i, n, P, eta, dpsi_lopc, DV_R;

	if (vars->MCCIndicator == false)
	{
		if (vars->TLIIndicator == true)
		{
			//Set mass
			//Call TLIBRN
			//Call ELEMT
			return false;
		}
		else
		{
			goto TLMCC_Conic_F5;
		}
	}

	dv_mcc = var[0];
	dgamma_mcc = var[1];
	dpsi_mcc = var[2];
	if (var.size() > 3)
	{
		gamma_loi = var[3];
	}
	else
	{
		gamma_loi = 0.0;
	}
	if (var.size() > 4)
	{
		dpsi_loi = var[4];
	}
	else
	{
		dpsi_loi = 0.0;
	}
	if (var.size() > 5)
	{
		DT_1st_pass = var[5]*3600.0;
	}
	else
	{
		DT_1st_pass = 0.0;
	}

	sv0 = vars->sv0;

	m_before_MCC = MEDQuantities.CSMMass;
	BURN(sv0.R, sv0.V, dv_mcc, dgamma_mcc, dpsi_mcc, isp_SPS, mfm0, RF, VF);
	m_before_LOI = m_before_MCC * mfm0;

	//TBD: MCOMP?
	
	if (KREF_MCC == 1)
	{
		R_patch = RF;
		V_patch = VF;
		MJD_patch = sv0.MJD;
		PATCH(R_patch, V_patch, MJD_patch, 1, 1);
	}
	else
	{
		R_patch = RF;
		V_patch = VF;
		MJD_patch = sv0.MJD;
	}
	beta = EBETA(R_patch, V_patch, mu_M, ainv);
	ainv_pl = ainv;
	XBETA(R_patch, V_patch, MJD_patch, beta, 2, R_pl, V_pl, MJD_pl);
	R_temp = R_pl;
	V_temp = V_pl;
	LIBRAT(R_temp, V_temp, MJD_pl, 4);
	H_pl = length(R_temp) - DataTable.R_lls;
	OrbMech::latlong_from_r(R_temp, lat_pl, lng_pl);

	lat_pl_stored = lat_pl;
	MJD_pl_stored = MJD_pl;
	h_pl_stored = H_pl;
	HH_pl = crossp(R_temp, V_temp);
	inc_pl = acos(HH_pl.z / length(HH_pl));

	if (vars->FreeReturnIndicator)
	{
		MATRIX3 Rot;
		VECTOR3 R_patch2, V_patch2, R_pg, V_pg, R_reentry, V_reentry, R_equ, V_equ, H_equ;
		double MJD_patch2, MJD_pg, H, E, beta, e, MJD_reentry;

		R_patch2 = R_pl;
		V_patch2 = V_pl;
		MJD_patch2 = MJD_pl;

		PATCH(R_patch2, V_patch2, MJD_patch2, 1, 2);
		beta = EBETA(R_patch2, V_patch2, mu_E, ainv);
		XBETA(R_patch2, V_patch2, MJD_patch2, beta, 1, R_pg, V_pg, MJD_pg);
		DGAMMA(length(R_pg), ainv, gamma_reentry, H, E, beta, e);
		XBETA(R_pg, V_pg, MJD_pg, beta, 1, R_reentry, V_reentry, MJD_reentry);
		H_fr_rtny = length(R_reentry) - R_E;
		Rot = OrbMech::GetObliquityMatrix(BODY_EARTH, MJD_reentry);
		R_equ = rhtmul(Rot, R_reentry);
		V_equ = rhtmul(Rot, V_reentry);
		H_equ = crossp(R_equ, V_equ);
		i_pg = inc_fr_stored = acos(H_equ.z / length(H_equ));

		if (vars->FreeReturnOnlyIndicator)
		{
			if (vars->FreeReturnInclinationIndicator)
			{
				if (vars->OptimizeIndicator)
				{
					arr[0] = H_pl;
					arr[1] = inc_pl;
					arr[2] = i_pg;
					arr[3] = H_fr_rtny;
					arr[4] = m_before_LOI;
					return false;
				}
				else
				{
					arr[0] = H_pl;
					arr[1] = inc_pl;
					arr[2] = H_fr_rtny;
					arr[3] = i_pg;

					if (arr.size() > 4)
					{
						arr[4] = lat_pl;
					}
					return false;
				}
			}
			else
			{
				arr[0] = H_pl;
				arr[1] = inc_pl;
				arr[2] = lat_pl;
				arr[3] = i_pg;
				arr[4] = H_fr_rtny;
				return false;
			}
		}
	}

	VECTOR3 R_LOI, V_LOI;
	double MJD_LOI;

	if (gamma_loi == 0.0)
	{
		R_LOI = R_pl;
		V_LOI = V_pl;
		MJD_LOI = MJD_pl;
	}
	else
	{
		double H, E, e;
		DGAMMA(length(R_pl), ainv_pl, gamma_loi, H, E, beta, e);
		XBETA(R_pl, V_pl, MJD_pl, beta, 2, R_LOI, V_LOI, MJD_LOI);
	}

	VECTOR3 H_LOI;
	double h_LOI, lat_LOI, lng_LOI, i_EMP, t_TL;

	h_LOI = length(R_LOI) - DataTable.R_lls;
	R_temp = R_LOI;
	V_temp = V_LOI;
	LIBRAT(R_temp, V_temp, MJD_LOI, 4);
	H_LOI = crossp(R_temp, V_temp);
	OrbMech::latlong_from_r(R_temp, lat_LOI, lng_LOI);
	if (lng_LOI < 0) lng_LOI += PI2;
	i_EMP = acos(H_LOI.z / length(H_LOI));
	t_TL = (MJD_LOI - sv0.MJD)*24.0*3600.0;

	if (!vars->HasLOI2Indicator && vars->FixedOrbitIndicator)
	{
		goto TLMCC_Conic_A1;
	}

	double a_LPO1, e_LPO1, v_pl_LPO, dv, a_LPO2, dt_bias, MJD_LPO;

	a_LPO1 = (2.0*DataTable.R_lls + MEDQuantities.H_A_LPO1 + MEDQuantities.H_P_LPO1) / 2.0;
	e_LPO1 = (MEDQuantities.H_A_LPO1 - MEDQuantities.H_P_LPO1) / (2.0*DataTable.R_lls + MEDQuantities.H_A_LPO1 + MEDQuantities.H_P_LPO1);
	v_pl_LPO = sqrt((1.0 + e_LPO1)*mu_M / ((1.0 - e_LPO1)*a_LPO1));
	dv = v_pl_LPO - length(V_LOI);
	BURN(R_LOI, V_LOI, dv, -gamma_loi, dpsi_loi, isp_MCC, mfm0, RF, VF);
	m_before_DOI = mfm0 * m_before_LOI;

	a_LPO2 = (2.0*DataTable.R_lls + MEDQuantities.H_A_LPO2 + MEDQuantities.H_P_LPO2) / 2.0;
	dt_bias = PI2*(sqrt(pow(a_LPO1, 3) / mu_M) - sqrt(pow(a_LPO2, 3) / mu_M))*MEDQuantities.Revs_LPO1;

	MJD_LPO = MJD_LOI + dt_bias / 24.0 / 3600.0;

	goto TLMCC_Conic_C4;

TLMCC_Conic_A1:

	VECTOR3 U_H, R_N, U_DS, u_pl;
	double r, v, theta, phi, gamma, psi, A_L, E_L, RA_LPO1, DV_LOI, dw_p, dh_a, dh_p, DT;

	RVIO(true, R_LOI, V_LOI, r, v, theta, phi, gamma, psi);
	R_N = unit(R_LOI);
	U_H = unit(crossp(R_LOI, V_LOI));

	if (vars->FirstSelect)
	{
		PRCOMP(R_LOI, V_LOI, MJD_LOI, RA_LPO1, A_L, E_L, u_pl, dw_p, dh_a, dh_p, DT, DT_1st_pass, SGSLOI);
		vars->FirstSelect = false;
	}

	U_DS = unit(crossp(SGSLOI.R, SGSLOI.V));

	VECTOR3 U_PJ, gamma_vec;
	double theta_var, R_NL, dh_n, V1, r_H, v_H, theta_H, phi_H, gamma_H, psi_H, r_N;

	U_PJ = unit(U_DS - R_N * dotp(U_DS, R_N));
	gamma_vec = unit(crossp(U_H, R_N));
	theta_var = length(crossp(U_DS, U_PJ))*OrbMech::sign(dotp(crossp(U_DS, U_PJ), gamma_vec));
	R_NL = A_L * (1.0 - E_L * E_L) / (1.0 + E_L * cos(MEDQuantities.TA_LOI));

	RVIO(true, R_LOI, V_LOI, r_H, v_H, theta_H, phi_H, gamma_H, psi_H);
	r_N = r_H; //TBD: ???
	dh_n = R_NL - r_H;
	V1 = sqrt(mu_M*(2.0 / r_N - 2.0 / (r_N + RA_LPO1)));
	DV_LOI = sqrt(v_H*v_H + V1 * V1 - 2.0*v_H*V1*(cos(gamma_H)*dotp(U_DS, U_H))) + 10.0*0.3048; //10 ft/s calibration DV to account for finite burn loss
	m_before_DOI = MCOMP(DV_LOI, MEDQuantities.Config, MEDQuantities.useSPS, m_before_LOI);

	if (vars->AfterLOIStopIndicator)
	{
		arr[0] = theta;
		arr[1] = inc_pl;
		arr[2] = dh_n;
		arr[3] = H_fr_rtny;
		arr[4] = i_pg;
		arr[5] = m_before_DOI;
		return false;
	}

	goto TLMCC_Conic_F5;
TLMCC_Conic_C4:
	
	VECTOR3 R_LPO, V_LPO, R_LLS, V_LLS;
	double MJD_LLS, lat_S, lng_S, psi_S;

	SCALE(RF, VF, MEDQuantities.H_A_LPO2, MEDQuantities.H_P_LPO2, R_LPO, V_LPO);
	MJD_LLS = MJD_LPO + DT_1st_pass / 24.0 / 3600.0;
	CTBODY(R_LPO, V_LPO, MJD_LPO, MJD_LLS, 2, mu_M, R_LLS, V_LLS);
	R_temp = R_LLS;
	V_temp = V_LLS;
	LIBRAT(R_temp, V_temp, MJD_LLS, 5);
	OrbMech::latlong_from_r(R_temp, lat_S, lng_S);
	RVIO(true, R_temp, V_temp, r, v, theta, phi, gamma, psi_S);
	ELEMT(R_LLS, V_LLS, mu_M, H, a, e, i, n, P, eta);
	P = PI2 / (length(V_LLS) / length(R_LLS) + OrbMech::w_Moon);
	VECTOR3 R_LLS2, V_LLS2;
	double MJD_LLS2, m_before_TEI;
	LOPC(R_LLS, V_LLS, MJD_LLS, unit(R_temp), Constants.m, Constants.n, P, R_LLS2, V_LLS2, MJD_LLS2, mfm0, dpsi_lopc);
	if (vars->FixedOrbitIndicator == false && abs(dpsi_lopc) < 8e-3)
	{
		DV_R = 2.0*length(V_LLS2)*(31.24975000037*dpsi_lopc*dpsi_lopc + 2.0000053333202e-3);
		mfm0 = exp(-DV_R / isp_MCC);
	}
	m_before_TEI = (m_before_DOI - MEDQuantities.LMMass)*mfm0;

TLMCC_Conic_F5:

	if (vars->FreeReturnIndicator)
	{
		arr[0] = H_pl;
		arr[1] = inc_pl;
		arr[2] = h_LOI;
		arr[3] = H_fr_rtny;
		arr[4] = i_pg;
		arr[5] = lat_S;
		arr[6] = lng_S;
		arr[7] = psi_S;
		if (vars->OptimizeIndicator)
		{
			arr[8] = m_before_DOI;
		}
		mass_stored = m_before_DOI;
	}
	else
	{
		arr[0] = H_pl;
		arr[1] = inc_pl;
		arr[2] = h_LOI;
		arr[3] = lat_S;
		arr[4] = lng_S;
		arr[5] = psi_S;
		if (vars->OptimizeIndicator)
		{
			arr[6] = m_before_DOI;
		}
	}
	return false;
}

double TLMCCProcessor::EBETA(VECTOR3 R, VECTOR3 V, double mu, double &ainv)
{
	double D0, r0, v0, a, beta;

	r0 = length(R);
	v0 = length(V);
	D0 = dotp(R, V);
	ainv = 2.0 / r0 - v0 * v0 / mu;
	a = 1.0 / ainv;

	if (a > 0)
	{
		double ecos_E, esin_E, E;

		ecos_E = 1.0 - r0 / a;
		esin_E = D0 / sqrt(mu*a);
		E = atan2(esin_E, ecos_E);
		beta = -E * sqrt(a);
	}
	else if (a < 0)
	{
		double ecosh_H, esinh_H, H;

		ecosh_H = 1.0 - r0 / a;
		esinh_H = D0 / sqrt(mu*abs(a));
		H = log((ecosh_H + esinh_H) / sqrt(abs(ecosh_H*ecosh_H - esinh_H * esinh_H)));
		beta = -H * sqrt(abs(a));
	}
	else
	{
		beta = -D0 / sqrt(mu);
	}
	return beta;
}

bool TLMCCProcessor::PATCH(VECTOR3 &R, VECTOR3 &V, double &MJD, int Q, int KREF)
{
	//KREF = 1: Earth reference input, 2: Moon reference input

	VECTOR3 R_EM, V_EM, R_ES, R1, V1, R2, V2, R21, A2;
	double beta, Ratio_desired, MJDF, r1, r2, r21, mu1, mu2, d1, d2, v12, v22, DRatioDBeta, DDRatioDDBeta, Ratio, DRatio, dbeta;
	int i = 0;

	if (KREF == 1)
	{
		if (RBETA(R, V, 40.0*R_E, Q, mu_E, beta))
		{
			return true;
		}
		Ratio_desired = 0.275;
		mu1 = mu_E;
		mu2 = mu_M;
	}
	else
	{
		if (RBETA(R, V, 10.0*R_E, Q, mu_M, beta))
		{
			return true;
		}
		Ratio_desired = 1.0 / 0.275;
		mu1 = mu_M;
		mu2 = mu_E;
	}

	do
	{
		XBETA(R, V, MJD, beta, KREF, R1, V1, MJDF);
		if (EPHEM(MJDF, R_EM, V_EM, R_ES))
		{
			return true;
		}
		if (KREF == 1)
		{
			R2 = R1 - R_EM;
			V2 = V1 - V_EM;
			R21 = R_EM;
		}
		else
		{
			R2 = R1 + R_EM;
			V2 = V1 + V_EM;
			R21 = -R_EM;
		}
		r1 = length(R1);
		r2 = length(R2);
		/*if (KREF == 1)
		{
			if (r1 > 60.0*R_E) return false;
		}
		else
		{
			if (r1 > 15.0*R_E) return false;
		}*/
		Ratio = r2 / r1;
		DRatio = Ratio_desired - Ratio;
		if (abs(DRatio) < 10e-12)
		{
			break;
		}
		r21 = length(R21);
		d1 = dotp(R1, V1);
		d2 = dotp(R2, V2);
		v12 = dotp(V1, V1);
		v22 = dotp(V2, V2);
		A2 = -R1 * mu1 / pow(r1, 3) + R21 * (mu1 + mu2) / pow(r21, 3);
		DRatioDBeta = 1.0 / r2 / sqrt(mu1)*(d2 - r2 * r2*d1 / r1 / r1);
		DDRatioDDBeta = r1 / mu1 * (v22 + dotp(R2, A2)) / r2 - d1 * d2 / (mu1*r1*r2) - d2 * d2*r1 / (mu1*pow(r2, 3)) - r2 * v12 / r1 / mu1 + r2 / r1 / r1 + 2.0*d1*d1*r2 / (mu1*pow(r1, 3));
		dbeta = 2.0*DRatio / (DRatioDBeta + DRatioDBeta / abs(DRatioDBeta)*sqrt(DRatioDBeta*DRatioDBeta + 2.0*DRatio*DDRatioDDBeta));
		beta = beta + dbeta;
		i++;
	} while (i < 10);

	R = R2;
	V = V2;
	MJD = MJDF;

	return false;
}


bool TLMCCProcessor::RBETA(VECTOR3 R0, VECTOR3 V0, double r, int Q, double mu, double &beta)
{
	double D0, r0, v0, ainv, e, QD, temp;

	//Q: +1 = Solution will be ahead of initial position with respect to direction of motion, -1 = solution behind initial position
	if (Q == -1)
	{
		QD = -1.0;
	}
	else if (Q == 1)
	{
		QD = 1.0;
	}
	else
	{
		return true;
	}

	r0 = length(R0);
	v0 = length(V0);
	D0 = dotp(R0, V0);
	ainv = 2.0 / r0 - v0 * v0 / mu;
	e = sqrt(pow(1.0 - r0 * ainv, 2) + D0 * D0*ainv / mu);
	if (e < 0.000001)
	{
		return true;
	}

	//Hyperbolic
	if (ainv < 0)
	{
		double H0;
		double cosh_H0 = 1.0 / e * (1.0 - r0 * ainv);
		double cosh_H = 1.0 / e * (1.0 - r * ainv);

		if (cosh_H0 < 1.0)
		{
			H0 = 0.0;
		}
		else
		{

			temp = cosh_H0 * cosh_H0 - 1.0;
			if (temp < 0)
			{
				return true;
			}
			H0 = D0 / abs(D0)*log(cosh_H0 + sqrt(temp));
		}
		temp = cosh_H * cosh_H - 1.0;
		if (temp < 0)
		{
			return true;
		}
		double H = log(cosh_H + sqrt(temp));
		double theta = H0 - QD * H;
		beta = QD * abs(theta)*sqrt(abs(1.0 / ainv));
	}
	//Elliptical
	else if (ainv > 0)
	{
		double cos_E0 = 1.0 / e * (1.0 - r0 * ainv);
		double cos_E = 1.0 / e * (1.0 - r * ainv);
		temp = max(0.0, 1.0 - cos_E0 * cos_E0);
		if (temp < 0)
		{
			return true;
		}
		double E0 = D0 / abs(D0)*atan(sqrt(temp) / cos_E0);
		temp = 1.0 - cos_E * cos_E;
		if (temp < 0)
		{
			return true;
		}
		double E = atan(sqrt(temp) / cos_E);
		double theta = E0 - QD * E;
		beta = QD * abs(theta)*sqrt(1.0 / ainv);
	}
	//Parabolic
	else
	{
		beta = D0 / sqrt(mu) + QD * sqrt(D0*D0 / mu + 2.0*(r - r0));
	}

	return false;
}

void TLMCCProcessor::XBETA(VECTOR3 R0, VECTOR3 V0, double MJD0, double beta, int K, VECTOR3 &RF, VECTOR3 &VF, double &MJDF)
{
	double D0, r0, v0, ainv, mu, a, F1, F2, F3, F4, t, r, f, g, fdot, gdot;

	mu = GetMU(K);
	r0 = length(R0);
	v0 = length(V0);
	D0 = dotp(R0, V0);
	ainv = 2.0 / r0 - v0 * v0 / mu;
	a = -beta * beta*ainv;
	FCOMP(a, F1, F2, F3, F4);
	t = (beta*beta*F1 + D0 * beta*F2 / sqrt(mu) + r0 * F3)*beta / sqrt(mu);
	MJDF = MJD0 + t / 24.0 / 3600.0;
	r = (D0*F3 / sqrt(mu) + beta * F2)*beta + r0 * F4;
	f = 1.0 - beta * beta*F2 / r0;
	g = t - pow(beta, 3)*F1 / sqrt(mu);
	fdot = -sqrt(mu)*beta*F3 / (r0 * r);
	gdot = 1.0 - beta * beta*F2 / r;
	RF = R0 * f + V0 * g;
	VF = R0 * fdot + V0 * gdot;
}

double TLMCCProcessor::GetMU(int k)
{
	if (k == 1)
	{
		return mu_E;
	}
	else
	{
		return mu_M;
	}
}

void TLMCCProcessor::FCOMP(double a, double &F1, double &F2, double &F3, double &F4)
{
	/*static const double xarr[] = { 2e-7,2e-5,2e-3,2e-2,2e-1,1,2,4,8,16,32,64,128,256,512 };
	static const int yarr[] = { 5,6,7,8,9,10,11,13,15,18,21,25,30,38,46 };

	unsigned n;
	unsigned i = 0;

	while (i < 14 && abs(a)>=xarr[i])
	{
		i++;
	}
	n = yarr[i];
	F1 = F2 = 0.0;

	for (i = 0;i < n;i++)
	{
		F1 = F1 + pow(a, i) / factorial(2 * i + 4 - 1);
		F2 = F2 + pow(a, i) / factorial(2 * i + 4 - 2);
	}*/

	F1 = OrbMech::stumpS(-a);
	F2 = OrbMech::stumpC(-a);

	F3 = a * F1 + 1.0;
	F4 = a * F2 + 1.0;
}

bool TLMCCProcessor::EPHEM(double MJD, VECTOR3 &R_EM, VECTOR3 &V_EM, VECTOR3 &R_ES)
{
	return (OrbMech::PLEFEM(*ephemeris, MJD, R_EM, V_EM, R_ES) == false);
}

bool TLMCCProcessor::LIBRAT(VECTOR3 &R, double MJD, int K)
{
	VECTOR3 V = _V(0, 0, 0);
	return LIBRAT(R, V, MJD, K);
}

bool TLMCCProcessor::LIBRAT(VECTOR3 &R, VECTOR3 &V, double MJD, int K)
{
	//Options
	//1: EMP to selenographic
	//2: Selenographic to EMP
	//3: EMP to selenocentric
	//4: Selenocentric to EMP
	//5: Selenocentric to selenographic
	//6: Selenographic to selenocentric

	if (K < 5)
	{
		MATRIX3 A;
		VECTOR3 R_ME, V_ME, R_ES, i, j, k;

		if (EPHEM(MJD, R_ME, V_ME, R_ES))
		{
			return true;
		}
		i = -unit(R_ME);
		k = unit(crossp(R_ME, V_ME));
		j = crossp(k, i);
		A = _M(i.x, j.x, k.x, i.y, j.y, k.y, i.z, j.z, k.z);

		if (K == 1 || K == 3)
		{
			R = mul(A, R);
			V = mul(A, V);
		}
		else
		{
			R = tmul(A, R);
			V = tmul(A, V);
		}

		if (K == 1 || K == 2)
		{
			MATRIX3 Rot;

			Rot = OrbMech::GetRotationMatrix(BODY_MOON, MJD);

			if (K == 1)
			{
				R = rhtmul(Rot, R);
				V = rhtmul(Rot, V);
			}
			else
			{
				R = rhmul(Rot, R);
				V = rhmul(Rot, V);
			}
		}
	}
	else
	{
		MATRIX3 Rot;

		Rot = OrbMech::GetRotationMatrix(BODY_MOON, MJD);

		if (K == 5)
		{
			R = rhtmul(Rot, R);
			V = rhtmul(Rot, V);
		}
		else
		{
			R = rhmul(Rot, R);
			V = rhmul(Rot, V);
		}
	}

	return false;
}

bool TLMCCProcessor::CTBODY(VECTOR3 R0, VECTOR3 V0, double MJD0, double MJDF, int K, double mu, VECTOR3 &RF, VECTOR3 &VF)
{
	double F1, F2, F3, F4, alpha;
	return CTBODY(R0, V0, MJD0, MJDF, K, mu, alpha, F1, F2, F3, F4, RF, VF);
}

bool TLMCCProcessor::CTBODY(VECTOR3 R0, VECTOR3 V0, double MJD0, double MJDF, int K, double mu, double &alpha, double &F1, double &F2, double &F3, double &F4, VECTOR3 &RF, VECTOR3 &VF)
{
	double dt;
	int i = 0;

	dt = (MJDF - MJD0)*24.0*3600.0;

	if (abs(dt) < 3.6e-9)
	{
		RF = R0;
		VF = V0;
		return false;
	}

	double r0, v0, ainv, D0, beta, t, r;

	r0 = length(R0);
	v0 = length(V0);
	ainv = 2.0 / r0 - v0 * v0 / mu;
	D0 = dotp(R0, V0);
	//Initial guess

	OrbMech::rv_from_r0v0(R0, V0, dt, RF, VF, mu);
	beta = 1.0 / 5.0*dt*sqrt(mu) / r0;
	do
	{
		alpha = -beta * beta*ainv;
		FCOMP(alpha, F1, F2, F3, F4);
		t = (beta*beta*F1 + D0 / sqrt(mu)*beta*F2 + r0 * F3)*beta / sqrt(mu);
		r = D0 / sqrt(mu)*beta*F3 + beta * beta*F2 + r0 * F4;
		beta = beta + (dt - t)*sqrt(mu) / r;
		i++;
		if (i == 20)
		{
			return true;
		}
	} while (abs((t - dt) / dt) >= 10e-12);

	double f, g, fdot, gdot;

	f = 1.0 - beta * beta*F2 / r0;
	g = t - pow(beta, 3)*F1 / sqrt(mu);
	fdot = -sqrt(mu)*beta*F3 / (r0 * r);
	gdot = 1.0 - beta * beta*F2 / r;
	RF = R0 * f + V0 * g;
	VF = R0 * fdot + V0 * gdot;

	if (K == 1 || ainv < 0.0)
	{
		return false;
	}

	VECTOR3 G0, Gdot0;

	G0 = RF;
	Gdot0 = VF;
	LIBRAT(G0, Gdot0, MJDF, 5);
	double n1, n2, n;

	n1 = Gdot0.z*G0.x - G0.z*Gdot0.x;
	n2 = Gdot0.z*G0.y - G0.z*Gdot0.y;
	n = sqrt(n1*n1 + n2 * n2);
	if (n <= 10e-12)
	{
		return false;
	}

	VECTOR3 H;
	double cos_Omega, sin_Omega, cos_i, sin_i, dOmega, d, rr, vv;

	cos_Omega = n1 / n;
	sin_Omega = n2 / n;
	H = unit(crossp(G0, Gdot0));
	cos_i = H.z;
	sin_i = sqrt(H.x*H.x + H.y*H.y);
	dOmega = -1.5*OrbMech::J2_Moon*R_M*R_M*sqrt(mu_M)*cos_i*pow(ainv, 3)*sqrt(ainv)*dt;
	d = dotp(G0, Gdot0);
	rr = dotp(G0, G0);
	vv = dotp(Gdot0, Gdot0);

	VECTOR3 N, M, G, Gdot;
	N = _V(cos_Omega*cos(dOmega) - sin_Omega * sin(dOmega), sin_Omega*cos(dOmega) + cos_Omega * sin(dOmega), 0.0);
	M = _V(-cos_i*(sin_Omega*cos(dOmega) + cos_Omega * sin(dOmega)), cos_i*(cos_Omega*cos(dOmega) - sin_Omega * sin(dOmega)), sin_i);
	G = N * (Gdot0.z*rr - G0.z*d) / n + M * G0.z*length(crossp(G0, Gdot0)) / n;
	Gdot = N * (Gdot0.z*d - G0.z*vv) / n + M * Gdot0.z*length(crossp(G0, Gdot0)) / n;
	LIBRAT(G, Gdot, MJDF, 6);
	RF = G;
	VF = Gdot;

	return false;
}

bool TLMCCProcessor::DGAMMA(double r0, double ainv, double gamma, double &H, double &E, double &beta, double &e)
{
	double c;

	H = 0.0;
	E = 0.0;
	e = 1.0 - r0 * ainv;
	c = sqrt(abs(2.0*r0*ainv - r0 * r0*ainv*ainv));

	if (ainv < 0)
	{
		H = log(c / e * tan(gamma) + sqrt(1.0 + pow(c*tan(gamma) / e, 2)));
		beta = H * sqrt(abs(1.0 / ainv));
	}
	else if (ainv > 0)
	{
		double sin_E = c * tan(gamma) / e;
		if (abs(sin_E) > 1.0)
		{
			return true;
		}
		E = atan(sin_E / sqrt(1.0 - sin_E * sin_E));
		beta = E * sqrt(1.0 / ainv);
	}
	else
	{
		beta = sin(gamma) / cos(gamma)*sqrt(2.0*r0);
	}

	return false;
}

void TLMCCProcessor::BURN(VECTOR3 R, VECTOR3 V, double dv, double dgamma, double dpsi, double isp, double &mfm0, VECTOR3 &RF, VECTOR3 &VF)
{
	double v_c, dv_r;
	BURN(R, V, 3, 0.0, 0.0, dv, dgamma, dpsi, isp, 0.0, v_c, dv_r, mfm0, RF, VF);
}

void TLMCCProcessor::BURN(VECTOR3 R, VECTOR3 V, int opt, double gamma0, double v_pl, double dv, double dgamma, double dpsi, double isp, double mu, double &v_c, double &dv_R, double &mfm0, VECTOR3 &RF, VECTOR3 &VF)
{
	double r, v;
	RF = R;

	r = length(R);
	v = length(V);

	if (opt == 1)
	{
		v_c = sqrt(mu / r);
		dv = v_c - v;
		dgamma = -gamma0;
	}
	else if (opt == 2)
	{
		dv = v_pl - v;
		dgamma = -gamma0;
	}

	VECTOR3 Rdot1, Rdot2;
	double d, h;

	d = dotp(R, V);
	h = length(crossp(R, V));
	Rdot1 = V * cos(dgamma) + (R*v*v - V * d) / h * sin(dgamma);
	Rdot2 = R * 2.0*dotp(R, Rdot1) / r / r * pow(sin(dpsi / 2.0), 2) + Rdot1 * cos(dpsi) - crossp(R, Rdot1) / r * sin(dpsi);
	VF = Rdot2 * (1.0 + dv / v);
	dv_R = sqrt(dv*dv + 4.0*v*(v + dv)*(pow(sin(dgamma / 2.0), 2) + (h*h*cos(dgamma) - h * d*sin(dgamma)) / (r*r*v*v)*pow(sin(dpsi / 2.0), 2)));
	mfm0 = exp(-dv_R / isp);
}

void TLMCCProcessor::RVIO(bool vecinp, VECTOR3 &R, VECTOR3 &V, double &r, double &v, double &theta, double &phi, double &gamma, double&psi)
{
	if (vecinp)
	{
		r = length(R);
		v = length(V);
		phi = asin(R.x / r);
		theta = atan2(R.y, R.x);
		gamma = asin(dotp(R, V) / r / v);
		psi = atan2(R.x*V.y - R.y*V.x, V.z*r - R.z*dotp(R, V) / r);
	}
	else
	{
		R = _V(cos(phi)*cos(theta), cos(phi)*sin(theta), sin(phi))*r;
		V = mul(_M(cos(phi)*cos(theta), -sin(theta), -sin(phi)*cos(theta), cos(phi)*sin(theta), cos(theta), -sin(phi)*sin(theta), sin(phi), 0, cos(phi)), _V(sin(gamma), cos(gamma)*sin(psi), cos(gamma)*cos(psi))*v);
	}
}

double TLMCCProcessor::MCOMP(double dv, bool docked, bool useSPS, double m0)
{
	if (docked == false || useSPS)
	{
		return m0 * exp(-abs(dv) / isp_SPS);
	}
	else
	{
		return m0 * exp(-abs(dv) / isp_DPS);
	}
}

void TLMCCProcessor::RNTSIM(VECTOR3 R, VECTOR3 V, double MJD, double lng_L, double &lat, double &lng, double &dlng)
{
	MATRIX3 Rot;
	VECTOR3 P, S, S_equ;
	double r, v, theta;

	r = length(R);
	v = length(V);

	P = V / v / cos(gamma_reentry) - R / r * tan(gamma_reentry);
	theta = Reentry_range / 3443.933585;
	S = R / r * cos(theta) + P * sin(theta);
	Rot = OrbMech::GetRotationMatrix(BODY_EARTH, MJD + Reentry_dt / 24.0 / 3600.0);
	S_equ = rhtmul(Rot, S);
	OrbMech::latlong_from_r(S_equ, lat, lng);
	dlng = lng - lng_L;
	if (dlng > PI)
	{
		dlng -= PI2;
	}
	if (dlng < -PI)
	{
		dlng += PI2;
	}
}

void TLMCCProcessor::LOPC(VECTOR3 R0, VECTOR3 V0, double MJD0, VECTOR3 L, int m, int n, double P, VECTOR3 &R3, VECTOR3 &V3, double &MJD3, double &mfm0, double &dpsi)
{
	VECTOR3 R1, V1, R2, V2;
	double dt1, MJD1, MJD2, dt2;

	dt1 = P * (-0.25 + (double)m);
	MJD1 = MJD0 + dt1 / 24.0 / 3600.0;
	CTBODY(R0, V0, MJD0, MJD1, 2, mu_M, R1, V1);
	dt2 = P * (double)(m + n);
	MJD2 = MJD0 + dt2 / 24.0 / 3600.0;
	CTBODY(R0, V0, MJD0, MJD2, 2, mu_M, R2, V2);
	LIBRAT(R2, V2, MJD2, 5);
	dpsi = -asin(dotp(unit(crossp(R2, V2)), L));
	BURN(R1, V1, 0.0, 0.0, dpsi, isp_MCC, mfm0, R3, V3);
	MJD3 = MJD1;
}

void TLMCCProcessor::PRCOMP(VECTOR3 R_nd, VECTOR3 V_nd, double MJD_nd, double &RA_LPO1, double &A_L, double &E_L, VECTOR3 &u_pl, double &dw_p, double &dh_a, double &dh_p, double &DT, double &DT_1st_pass, MPTSV &SGSLOI)
{
	VECTOR3 u_lls, u_pc, h_pc, RR2, VV2, R_L, V_L, RR_LPO1, VV_LPO1, u_lls_equ;
	double RP_LPO1, a_lls, dt_LLS, MJD_LLS, da_apo, R1, DR1, da, DA, DA_apo, e, R2, phi2, lambda2, psi2, V2, gamma2, eta2, dt3, dt, MJDI, dt2;
	double a_LPO1, e_LPO1, r, v, gamma, psi, theta, phi;

	u_pc = unit(R_nd);
	h_pc = unit(crossp(R_nd, V_nd));

	DR1 = modf(MEDQuantities.Revs_LPO1, &R1);

	RA_LPO1 = MEDQuantities.H_A_LPO1 + DataTable.R_lls;
	RP_LPO1 = MEDQuantities.H_P_LPO1 + DataTable.R_lls;
	a_lls = DataTable.R_lls + (MEDQuantities.H_A_LPO2 + MEDQuantities.H_P_LPO2) / 2.0;

	a_LPO1 = (RA_LPO1 + RP_LPO1) / 2.0;
	e_LPO1 = (RA_LPO1 - RP_LPO1) / (RA_LPO1 + RP_LPO1);

	r = a_LPO1 * (1.0 - e_LPO1 * e_LPO1) / (1.0 + e_LPO1 * cos(MEDQuantities.TA_LOI));
	v = sqrt(mu_M*(2.0 / r - 1.0 / a_LPO1));
	if (MEDQuantities.TA_LOI != 0.0)
	{
		gamma = -OrbMech::sign(MEDQuantities.TA_LOI)*acos(sqrt(mu_M*a_LPO1*(1.0 - e_LPO1 * e_LPO1)) / r / v);
	}
	else
	{
		gamma = 0.0;
	}
	psi = 0.0;
	theta = 0.0;
	phi = 0.0;
	RVIO(false, RR_LPO1, VV_LPO1, r, v, theta, phi, gamma, psi);
	dt2 = OrbMech::time_theta(RR_LPO1, VV_LPO1, DR1, mu_M);
	eta2 = MEDQuantities.TA_LOI + DR1;
	if (eta2 < 0)
	{
		eta2 += PI2;
	}

	dt_LLS = PI2 / sqrt(mu_M)*(R1*pow((RA_LPO1 + RP_LPO1) / 2.0, 1.5) + MEDQuantities.Revs_LPO2 * pow(a_lls, 1.5)) + dt2;
	MJD_LLS = MJD_nd + dt_LLS / 24.0 / 3600.0;
	u_lls_equ = OrbMech::r_from_latlong(DataTable.lat_lls, DataTable.lng_lls);
	for (int i = 0;i < 2;i++)
	{
		u_lls = u_lls_equ;
		LIBRAT(u_lls, MJD_LLS, 6);
		da_apo = acos(dotp(u_lls, u_pc));
		if (dotp(crossp(u_lls, u_pc), h_pc) > 0)
		{
			da = PI2 - da_apo;
		}
		else
		{
			da = da_apo;
		}
		DA_apo = da - DR1;
		if (DA_apo > 0)
		{
			DA = DA_apo;
		}
		else
		{
			DA = DA_apo + PI2;
		}
		e = -1.0 + (MEDQuantities.H_A_LPO2 + DataTable.R_lls) / a_lls;
		phi2 = DataTable.lat_lls;
		lambda2 = DataTable.lng_lls;
		psi2 = DataTable.psi_lls;
		R2 = a_lls * (1.0 - e * e) / (1.0 + e * cos(-MEDQuantities.site_rotation_LPO2));
		V2 = sqrt(mu_M*(2.0 / R2 - 1.0 / a_lls));
		gamma2 = -OrbMech::sign(-MEDQuantities.site_rotation_LPO2)*acos(sqrt(mu_M*a_lls*(1.0 - e * e)) / R2 / V2);
		RVIO(false, RR2, VV2, R2, V2, lambda2, phi2, gamma2, psi2);

		dt3 = OrbMech::time_theta(RR2, VV2, DA, mu_M);
		MJD_LLS = MJD_nd + (dt_LLS + dt3) / 24.0 / 3600.0;
	}
	
	LIBRAT(RR2, VV2, MJD_LLS, 6);

	dt = -(OrbMech::period(RR2, VV2, mu_M)*MEDQuantities.Revs_LPO2 + dt3);
	OrbMech::oneclickcoast(RR2, VV2, MJD_LLS, dt, R_L, V_L, hMoon, hMoon);
	MJDI = MJD_LLS + dt / 24.0 / 3600.0;

	VECTOR3 H_L, R1I, V1I, Rtemp, Vtemp, u_pl_apo;
	double i_L, n, P, eta, RA2, RP2, r_L, v_L, gamma_L, E, A, V1, gamma1, lambda, ddh, RA1, RP1;

	ELEMT(R_L, V_L, mu_M, H_L, A_L, E_L, i_L, n, P, eta);
	RVIO(true, R_L, V_L, r_L, v_L, lambda, phi, gamma, psi);
	RA2 = (1.0 + E_L)*A_L;
	RP2 = (1.0 - E_L)*A_L;
	gamma_L = asin(dotp(R_L, V_L) / r_L / v_L);
	u_pl = unit(crossp(V_L, crossp(R_L, V_L)) / mu_M - R_L / length(R_L));

	ddh = 0.0; //TBD

	E = (RA_LPO1 - (r_L - ddh)) / (RA_LPO1 + (r_L - ddh)*cos(eta2));
	A = RA_LPO1 / (1.0 + E);
	V1 = sqrt(mu_M * (2.0 / (r_L - ddh) - 1.0 / A));
	gamma1 = OrbMech::sign(eta2)*acos(sqrt(mu_M*A*(1.0 - E * E)) / ((r_L - ddh)*V1));
	RVIO(false, Rtemp, Vtemp, r_L, v_L, lambda, phi, gamma1, psi);
	dt = (MJD_nd - MJDI)*24.0*3600.0;
	OrbMech::oneclickcoast(Rtemp, Vtemp, MJDI, dt, R1I, V1I, hMoon, hMoon);
	u_pl_apo = unit(crossp(V1I, crossp(R1I, V1I)) / mu_M - R1I / length(R1I));
	ELEMT(R1I, V1I, mu_M, H_L, A_L, E_L, i_L, n, P, eta);
	RA1 = (1.0 + E_L)*A_L;
	RP1 = (1.0 - E_L)*A_L;

	dw_p = acos(dotp(u_pl, u_pl_apo))*OrbMech::sign(dotp(crossp(u_pl_apo, u_pl), crossp(R1I, V1I)));
	dh_a = RA2 - RA1;
	dh_p = RP2 - RP1;
	DT = (MJDI - MJD_nd)*24.0*3600.0;
	DT_1st_pass = (MJD_LLS - MJD_nd)*24.0*3600.0;

	SGSLOI.gravref = hMoon;
	SGSLOI.MJD = MJD_nd;
	SGSLOI.R = R1I;
	SGSLOI.V = V1I;
}

void TLMCCProcessor::ELEMT(VECTOR3 R, VECTOR3 V, double mu, VECTOR3 &H, double &a, double &e, double &i, double &n, double &P, double &eta)
{
	double ainv, r, v, h;

	r = length(R);
	v = length(V);
	ainv = 2.0 / r - v * v / mu;
	a = 1.0 / ainv;
	e = sqrt(pow(1.0 - r * ainv, 2) + pow(dotp(R, V), 2)*ainv / mu);
	H = crossp(R, V);
	h = length(H);
	i = acos(H.z / h);
	n = pow(mu, 0.5) / pow(abs(a), 1.5);
	if (e != 0.0)
	{
		eta = atan2(h*dotp(R, V), pow(h, 2) - mu * r);
	}
	if (e < 1.0)
	{
		P = PI2 / n;
	}
}

void TLMCCProcessor::SCALE(VECTOR3 R0, VECTOR3 V0, double H_A, double H_P, VECTOR3 &RF, VECTOR3 &VF)
{
	VECTOR3 h;
	double v_ap, r_ap,r_per, e, a;

	h = unit(crossp(R0, V0));
	r_ap = H_A + DataTable.R_lls;
	r_per = H_P + DataTable.R_lls;
	a = (r_per + r_ap) / 2.0;

	RF = unit(R0)*r_ap;
	e = 1.0 - 2.0 / (r_ap / r_per + 1.0);
	v_ap = sqrt((1.0 - e)*mu_M / ((1.0 + e)*a));
	VF = unit(crossp(h, RF))*v_ap;
}

double TLMCCProcessor::DDELTATIME(double a, double dt_apo, double xm, double betam, double dt)
{
	double ddt;

	a = a / R_E;
	dt_apo = dt_apo / 3600.0;
	xm = xm / R_E;
	dt = dt / 3600.0;

	if (a > 0.0 && a < 14.0)
	{
		a = 14.0;
	}
	else if (a > 91.0 || a < 0.0)
	{
		a = 91.0;
	}

	static const double b[] = { 0.48237399e1, -0.33327854e3, 0.98116717e4, -0.15785115e3, 0.42236397e-1, 0.50951511, 0.35644845, -0.15132136e-1, -0.77055739e2,
		0.32689925e1, 0.31964177e4, -0.16267514e1, -0.99746361e6, 0.16614352e-2, 0.10673486e4, -0.12140743e-2, 0.43980221e-2, -0.46467769e2, 0.44041338e-1, -0.23454849,
		0.55472146e7, -0.42499530e-4, 0.22349293e-3, -0.55596397e5, 0.28837746e-1, -0.91659504e-3, -0.53786773, 0.37944308e-2, 0.85832895e2,-0.69204019e-3 };

	ddt = b[0] + b[1] / a + b[2] / (a*a) + b[3] * dt_apo / a + b[4] * dt_apo*dt_apo + b[5] * dt_apo + b[6] * xm + b[7] * xm*dt_apo + b[8] * xm / a + b[9] * xm*dt_apo / a + b[10] * dt_apo / (a*a) + b[11] * dt_apo*dt_apo / a +
		b[12] / pow(a, 3) + b[13] * pow(dt_apo, 3) + b[14] * xm / (a*a) + b[15] * xm*dt_apo*dt_apo + b[16] * dt_apo*dt_apo*xm / a + b[17] * xm*dt_apo / (a*a) + b[18] * dt_apo*dt_apo*xm / (a*a) +
		b[19] * xm*cos(betam) + b[20] / pow(a, 4) + b[21] * pow(dt_apo, 3) + b[22] * pow(xm, 3) + b[23] * dt_apo / pow(a, 4) + b[24] * dt + b[25] * dt*dt + b[26] * dt*xm / a + b[27] * dt*xm + b[28] * dt / a +
		b[29] * dt*dt*xm*xm / (a*a);

	return ddt * 3600.0;
}