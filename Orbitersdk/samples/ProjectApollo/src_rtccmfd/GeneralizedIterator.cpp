/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Generalized Iterator

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
#include "GeneralizedIterator.h"

namespace GenIterator
{
	GeneralizedIteratorBlock::GeneralizedIteratorBlock()
	{
		for (int i = 0;i < 30;i++)
		{
			IndVarSwitch[i] = false;
			IndVarGuess[i] = 0.0;
			IndVarWeight[i] = 0.0;
			IndVarStep[i] = 0.0;
			DepVarSwitch[i] = false;
			DepVarLowerLimit[i] = 0.0;
			DepVarUpperLimit[i] = 0.0;
			DepVarClass[i] = 0;
			DepVarWeight[i] = 0.0;
		}
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

	void MatrixMultiply(double **P, const std::vector<double> &W_X, const std::vector<double> &W_Y, const std::vector<double> &dy, int m, int n, double **C, double *c)
	{
		//W_X is M
		//W_Y is N
		//P is NxM
		//dy, b are N
		//c is M
		//B is NxM
		//C is MxM

		double *b = new double[n];
		double **B = new double*[n];
		int i;

		for (i = 0;i < n;i++)
		{
			B[i] = new double[m];
		}

		vec_mul_vec(W_Y, dy, b);
		tmat_mul_vec(P, b, m, n, c);
		diag_mul_mat(W_Y, P, n, m, B);
		tmat_mul_mat(P, B, m, n, m, C);

		delete[] b;
		delete[] B;
	}

	void ComputeCoefficients(double **CARR, const std::vector<double> &W_X, double lambda, int m, int n, double **D)
	{
		//A is M
		//C is MxM
		//D is MxM
		double *A = new double[m];

		vec_mul_skal(W_X, lambda, A);
		mat_plus_vec(CARR, A, m, D);

		delete[] A;
	}

	bool SolveEquations(double **D, double *c, int m, std::vector<double> &dx)
	{
		int *PP = new int[m + 1];

		if (OrbMech::LUPDecompose(D, m, 0.0, PP) == 0)
		{
			return true;
		}
		OrbMech::LUPSolve(D, PP, c, m, dx);
		delete[] PP;
		return false;
	}

	void OpenRanks(std::vector<int> &xmap, std::vector<double> &in, std::vector<double> &out, int m)
	{
		for (int i = 0;i < m;i++)
		{
			out[xmap[i]] = in[i];
		}
	}

	void CloseRanks(std::vector<int> &ymap, std::vector<double> &in, std::vector<double> &out, int n2)
	{
		int j = 0;
		for (int i = 0;i < n2;i++)
		{
			if (ymap[j] == i)
			{
				out[j] = in[i];
				j++;
			}
			if (j == ymap.size())
			{
				break;
			}
		}
	}

	bool GeneralizedIterator(bool(*state_evaluation)(void*, std::vector<double>&, void*, std::vector<double>&, bool), GeneralizedIteratorBlock vars, void *constants, void *data, std::vector<double> &x_res, std::vector<double> &y_res)
	{
		double lambda, R, R_old, w_avg, **P, **CARR, *CVEC, **DARR;
		bool select = true, hasclass3, errind;
		int n, nMax, class1num, j_optm;
		unsigned N, M, i, j;
		std::vector<double> Target, var_star, v_l, *Y, var_star_temp, var_star_cur, Y_star, C, dx, dy, dy_temp, W_Y, W_Y_apo, W_X, step, LowerLimit, UpperLimit, trajin, trajout, depweight, borderinterval;
		std::vector<double> Y_star_best;
		std::vector<int> xmap, ymap, yclass, KPULL;

		trajin.assign(MGENITER, 0);
		trajout.assign(NGENITER, 0);

		//Establish arrays, size is the number of active independent variables
		M = 0;
		for (i = 0;i < MGENITER;i++)
		{
			if (vars.IndVarSwitch[i])
			{
				//Store weights
				W_X.push_back(vars.IndVarWeight[i]);
				//Set up initial guess
				var_star_temp.push_back(vars.IndVarGuess[i]);
				//Store steps
				step.push_back(vars.IndVarStep[i]);
				//Store index
				xmap.push_back(i);
				M++;
			}
			else
			{
				//Store constants
				trajin[i] = vars.IndVarGuess[i];
			}
		}
		//Establish arrays, size is the number of active dependent variables
		N = 0;
		for (i = 0;i < NGENITER;i++)
		{
			if (vars.DepVarSwitch[i])
			{
				LowerLimit.push_back(vars.DepVarLowerLimit[i]);
				UpperLimit.push_back(vars.DepVarUpperLimit[i]);
				//Calculate target as the average between min and max value
				Target.push_back((vars.DepVarUpperLimit[i] + vars.DepVarLowerLimit[i]) / 2.0);
				//Save class
				yclass.push_back(vars.DepVarClass[i]);
				//Store weights
				depweight.push_back(vars.DepVarWeight[i]);
				//Store index
				ymap.push_back(i);
				N++;
			}
		}

		//Set up a vector
		Y = new std::vector<double>[M];

		var_star.assign(M, 0);
		var_star_cur.assign(M, 0);
		dx.assign(M, 0);
		v_l.assign(M, 0);

		Y_star.assign(N, 0);
		Y_star_best.assign(N, 0);
		C.assign(N, 0);
		dy.assign(N, 0);
		dy_temp.assign(N, 0);
		W_Y.assign(N, 0);
		W_Y_apo.assign(N, 0);
		borderinterval.assign(N, 0);
		KPULL.assign(N, 0);
		j_optm = -1;

		for (i = 0;i < M;i++)
		{
			Y[i].assign(N, 0);
		}

		P = new double*[N];
		CARR = new double *[M];
		DARR = new double *[M];
		CVEC = new double[M];
		for (i = 0;i < M;i++)
		{
			CVEC[i] = 0.0;
			CARR[i] = new double[M];
			DARR[i] = new double[M];
			for (j = 0;j < M;j++)
			{
				CARR[i][j] = 0.0;
				DARR[i][j] = 0.0;
			}
		}
		for (i = 0;i < N;i++)
		{
			P[i] = new double[M];
			for (j = 0;j < M;j++)
			{
				P[i][j] = 0.0;
			}
		}

		//Set up iteration counters
		nMax = 100;
		n = 0;

		//Use initial guess to get a first vector
		OpenRanks(xmap, var_star_temp, trajin, M);
		errind = state_evaluation(data, trajin, constants, trajout, select);
		CloseRanks(ymap, trajout, Y_star, NGENITER);
		if (errind)
		{
			return true;
		}
		for (i = 0;i < N;i++)
		{
			dy[i] = Target[i] - Y_star[i];
		}

		//Initial guess
		lambda = pow(2, -28);

		//Select y weights based on class designation
		class1num = 0;
		hasclass3 = false;
		w_avg = 1.0;
		for (i = 0;i < N;i++)
		{
			if (yclass[i] == 1)
			{
				W_Y[i] = pow(2, -40) / pow((UpperLimit[i] - LowerLimit[i]) / 2.0, 2);
				class1num++;
				w_avg *= W_Y[i];
				C[i] = 1.0;
			}
			else if (yclass[i] == 3)
			{
				if (hasclass3)
				{
					//Error
				}
				else
				{
					j_optm = i;
					hasclass3 = true;
				}
			}
		}
		w_avg = pow(w_avg, 1.0 / ((double)(class1num)));
		for (i = 0;i < N;i++)
		{
			if (yclass[i] == 2)
			{
				W_Y[i] = depweight[i] * w_avg;
				borderinterval[i] = 0.002*abs(UpperLimit[i] - LowerLimit[i]);
			}
			else if (yclass[i] == 3)
			{
				W_Y[i] = depweight[i] * pow(10, -4)*pow(2, -40)*((double)(class1num)) / pow(dy[i], 2);
			}
		}

		// "In the select mode, search is made for variables which are inside the acceptable interval, but only just inside, that is, they are within
		// 0.2% of full range of one of the limits. When a variable has such a value, a move procedure begins. The limit near the value of the variable is
		// temporarily replaced by its opposite limit, thus shrinking the interval of acceptability to zero length. The residual vector and its length
		// are then recomputed. The move counter, which has been continually reset at every iteration, now begins to count, and control passes into the
		// basic iteration loop. As the iterations proceed, additional variables may have values which would start a move procedure. If this happens, the
		// limits are treated the same way, and the move counter begins to count from 1 again. Finally when the move counter reaches 6, all(?) the limits
		// are restored to their values, as originally input, and the(?) move counter reverts to being reset at every iteration.
		bool moving = false;
		//True if all class 1 and 2 variables have converged in residual check
		bool convergence = false;
		bool locked = false;
		bool sizing = false;
		int sizingcounter = 0;
		//See at "moving".
		int movingcounter = 0;
		bool skip = true;
		bool badresidual = false;
		std::vector<bool> movingarr, lockedarr;
		movingarr.assign(N, 0);
		lockedarr.assign(N, 0);
		R_old = 1000000.0;
		var_star = var_star_cur = var_star_temp;

		goto NewGeneralizedIterator_B;

	NewGeneralizedIterator_X:
		W_Y[j_optm] = depweight[j_optm] * pow(2, -32)*((double)(class1num)) / pow(dy[j_optm], 2);
		for (j = 0;j < N;j++)
		{
			if (yclass[j] == 1 || yclass[j] == 3)
			{
				KPULL[j] = 1;
			}
			else if (yclass[j] == 2)
			{
				KPULL[j] = 3;
			}
		}
		n = 0;
		skip = true;
	NewGeneralizedIterator_B:
		if (moving)
		{
		NewGeneralizedIterator_AA:
			moving = false;
			for (j = 0;j < N;j++)
			{
				if (movingarr[j])
				{
					Target[j] = (UpperLimit[j] + LowerLimit[j]) / 2.0;
					movingarr[j] = false;
				}
			}
		}
	NewGeneralizedIterator_EE:
		//Residual Computation and Checking
		convergence = true;
		R = 0.0;
		for (j = 0;j < N;j++)
		{
			dy[j] = Target[j] - Y_star[j];
			if (Y_star[j] >= LowerLimit[j] && Y_star[j] <= UpperLimit[j])
			{
				if (movingarr[j] || lockedarr[j])
				{
					W_Y_apo[j] = W_Y[j];
				}
				else
				{
					W_Y_apo[j] = C[j]*W_Y[j];
				}
			}
			else
			{
				if (j != j_optm)
				{
					convergence = false;
				}
				W_Y_apo[j] = W_Y[j];
				if (yclass[j] == 2)
				{
					double testval = 2.0;
				}
			}
			R = R + W_Y_apo[j] * dy[j] * dy[j];
		}
		if (skip == false)
		{
			goto NewGeneralizedIterator_F;
		}
	NewGeneralizedIterator_C:
		//Store information for last good nominal
		var_star = var_star_temp;
		Y_star_best = Y_star;
		R_old = R;
		if (skip)
		{
			goto NewGeneralizedIterator_E;
		}
		sizing = true;
		sizingcounter++;
		if (sizingcounter == 3)
		{
			sizing = false;
			goto NewGeneralizedIterator_H;
		}
		else
		{
			lambda /= 2.0;
			goto NewGeneralizedIterator_D;
		}
	NewGeneralizedIterator_E:
		skip = false;
		sizing = false;
		badresidual = false;
		sizingcounter = 0;
		var_star_cur = var_star;
		n++;
		if (n > nMax)
		{
			return true;
		}
		if (select)
		{
			if (lambda > pow(2, -49))
			{
				lambda /= 64.0;
			}
		}
		//Partial computation
		for (j = 0;j < M;j++)
		{
			//Evalue trajectory computer
			v_l = var_star;
			v_l[j] += step[j];

			OpenRanks(xmap, v_l, trajin, M);
			errind = state_evaluation(data, trajin, constants, trajout, select);
			CloseRanks(ymap, trajout, Y[j], NGENITER);

			if (errind)
			{
				return true;
			}

			//Calculate matrix valuess
			for (i = 0;i < N;i++)
			{
				P[i][j] = (Y[j][i] - Y_star[i]) / step[j];
			}
		}
		MatrixMultiply(P, W_X, W_Y_apo, dy, M, N, CARR, CVEC);
	NewGeneralizedIterator_D:
		ComputeCoefficients(CARR, W_X, lambda, M, N, DARR);
		if (SolveEquations(DARR, CVEC, M, dx) == false)
		{
			goto NewGeneralizedIterator_G;
		}
		if (lambda <= 1.0)
		{
			goto NewGeneralizedIterator_CC;
		}
	NewGeneralizedIterator_DD:
		if (select == false)
		{
			convergence = true;
			goto NewGeneralizedIterator_END;
		}
		if (moving == false)
		{
			return true;
		}
		moving = false;
		for (j = 0;j < N;j++)
		{
			movingarr[j] = false;
		}
		lambda = pow(2, -28);
		skip = true;
		goto NewGeneralizedIterator_EE;
	NewGeneralizedIterator_F:
		if (badresidual)
		{
			goto NewGeneralizedIterator_H;
		}
		if (select)
		{
			if (convergence)
			{
				if (j_optm >= 0)
				{
					select = false;
					goto NewGeneralizedIterator_X;
				}
				else
				{
					goto NewGeneralizedIterator_END;
				}
			}
		}
		if (R < R_old)
		{
			goto NewGeneralizedIterator_C;
		}
	NewGeneralizedIterator_CC:
		if (sizing)
		{
			var_star_cur = var_star;
			Y_star = Y_star_best;
			sizing = false;
			badresidual = true;
			goto NewGeneralizedIterator_EE;
		}
		lambda *= 8.0;
		goto NewGeneralizedIterator_D;
	NewGeneralizedIterator_G:
		convergence = true;
		for (i = 0;i < M;i++)
		{
			if (abs(dx[i]) > 65536.0*abs(step[i]))
			{
				goto NewGeneralizedIterator_CC;
			}
			if (abs(dx[i]) > step[i] / 65536.0)
			{
				convergence = false;
			}
			var_star_temp[i] = var_star_cur[i] + dx[i];
		}
		if (convergence)
		{
			goto NewGeneralizedIterator_DD;
		}
		OpenRanks(xmap, var_star_temp, trajin, M);
		errind = state_evaluation(data, trajin, constants, trajout, select);
		CloseRanks(ymap, trajout, Y_star, NGENITER);
		if (errind)
		{
			goto NewGeneralizedIterator_CC;
		}
		goto NewGeneralizedIterator_EE;
	NewGeneralizedIterator_H:
		//Barrier Computation
		if (select == false)
		{
			goto NewGeneralizedIterator_K;
		}
		if (moving)
		{
			movingcounter++;
			if (movingcounter == 6)
			{
				goto NewGeneralizedIterator_AA;
			}
		}
		skip = true;
		//Select mode barrier check
		for (j = 0;j < N;j++)
		{
			//Is Y_j an active class 2 variable?
			if (yclass[j] == 2)
			{
				//Is Y_j inside its limits?
				if (Y_star[j] >= LowerLimit[j] && Y_star[j] <= UpperLimit[j])
				{
					if (Y_star[j] < LowerLimit[j] + borderinterval[j])
					{
						if (movingarr[j] == false)
						{
							Target[j] = UpperLimit[j];
							movingarr[j] = true;
							skip = false;
						}
					}
					else if (Y_star[j] > UpperLimit[j] - borderinterval[j])
					{
						if (movingarr[j] == false)
						{
							Target[j] = LowerLimit[j];
							movingarr[j] = true;
							skip = false;
						}
					}
				}
			}
		}
		if (skip)
		{
			goto NewGeneralizedIterator_E;
		}
		movingcounter = 0;
		moving = true;
		R_old = 100.0;
		goto NewGeneralizedIterator_EE;

	NewGeneralizedIterator_K:
		//optimize mode barrier check
		skip = true;
		j = 0;
	NewGeneralizedIterator_T:
		switch (KPULL[j])
		{
		case 1: //Class 1 or 3 variables
			goto NewGeneralizedIterator_L;
		case 2:
			goto NewGeneralizedIterator_Q;
		case 3: //Class 2 variables
			goto NewGeneralizedIterator_P;
		case 4: //Lower Barrier
			if (Y_star[j] <= LowerLimit[j])
			{
				goto NewGeneralizedIterator_L;
			}
		NewGeneralizedIterator_R:
			KPULL[j]++;
			goto NewGeneralizedIterator_L;
			//2nd and 3rd iteration while the variable is inside the limit (lower)
		case 5:
		case 6:
			if (Y_star[j] > LowerLimit[j])
			{
				goto NewGeneralizedIterator_R;
			}
		NewGeneralizedIterator_S:
			KPULL[j]--;
			goto NewGeneralizedIterator_L;
		case 7:
			goto NewGeneralizedIterator_M;
		case 8: //Upper Barrier
			if (Y_star[j] >= UpperLimit[j])
			{
				goto NewGeneralizedIterator_L;
			}
			goto NewGeneralizedIterator_R;
		//2nd and 3rd iteration while the variable is inside the limit (upper)
		case 9:
		case 10:
			if (Y_star[j] < UpperLimit[j])
			{
				goto NewGeneralizedIterator_R;
			}
			goto NewGeneralizedIterator_S;
		case 11:
			goto NewGeneralizedIterator_N;
		}
	NewGeneralizedIterator_M:
		if (Y_star[j] <= LowerLimit[j])
		{
			goto NewGeneralizedIterator_S;
		}
		Target[j] = LowerLimit[j] + 8.0*borderinterval[j];
		KPULL[j] = 2;
		C[j] = 1.0;
		goto NewGeneralizedIterator_O;
	NewGeneralizedIterator_N:
		if (Y_star[j] >= UpperLimit[j])
		{
			goto NewGeneralizedIterator_S;
		}
		Target[j] = UpperLimit[j] - 8.0*borderinterval[j];
		KPULL[j] = 2;
		C[j] = 1.0;
		goto NewGeneralizedIterator_O;
	NewGeneralizedIterator_P:
		if (Y_star[j] <= LowerLimit[j] + borderinterval[j])
		{
			//Within 0.2% of lower limit
			KPULL[j] = 4;
			Target[j] = LowerLimit[j];
			lockedarr[j] = true;
			goto NewGeneralizedIterator_O;
		}
		else if (Y_star[j] >= UpperLimit[j] - borderinterval[j])
		{
			//Within 0.2% of upper limit
			KPULL[j] = 8;
			Target[j] = UpperLimit[j];
			lockedarr[j] = true;
			goto NewGeneralizedIterator_O;
		}
		goto NewGeneralizedIterator_L;
	NewGeneralizedIterator_Q:
		Target[j] = (UpperLimit[j] + LowerLimit[j]) / 2.0;
		//Put into class 2
		C[j] = 0.0;
		KPULL[j] = 3;
		lockedarr[j] = false;
	NewGeneralizedIterator_O:
		skip = false;
	NewGeneralizedIterator_L:
		if (j < N - 1)
		{
			j++;
			goto NewGeneralizedIterator_T;
		}
		if (skip)
		{
			goto NewGeneralizedIterator_E;
		}
		goto NewGeneralizedIterator_EE;

	NewGeneralizedIterator_END:
		delete[] Y;
		delete[] P;
		delete[] CARR;
		delete[] CVEC;

		x_res = var_star;
		y_res = Y_star_best;

		//Return true for error, false for convergence
		return (convergence == false);
	}
}