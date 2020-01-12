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