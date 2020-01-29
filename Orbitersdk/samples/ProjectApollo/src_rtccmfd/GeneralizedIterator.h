/****************************************************************************
This file is part of Project Apollo - NASSP

RTCC Generalized Iterator (Header)

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

#include <vector>

namespace GenIterator
{
	//Max number of independant variables
	const int MGENITER = 30;
	//Max number of dependant variables
	const int NGENITER = 30;

	struct GeneralizedIteratorBlock
	{
		GeneralizedIteratorBlock();
		bool IndVarSwitch[30];
		double IndVarGuess[30];
		double IndVarWeight[30];
		double IndVarStep[30];
		bool DepVarSwitch[30];
		double DepVarLowerLimit[30];
		double DepVarUpperLimit[30];
		int DepVarClass[30];
		double DepVarWeight[30];
	};

	void OpenRanks(std::vector<int> &xmap, std::vector<double> &in, std::vector<double> &out, int m);
	void CloseRanks(std::vector<int> &ymap, std::vector<double> &in, std::vector<double> &out, int n2);
	bool GeneralizedIterator(bool(*state_evaluation)(void *, std::vector<double>&, void*, std::vector<double>&, bool), GeneralizedIteratorBlock vars, void *constants, void *data, std::vector<double> &x_res, std::vector<double> &y_res);
	void MatrixMultiply(double **P, const std::vector<double> &W_X, const std::vector<double> &W_Y, const std::vector<double> &dy, int m, int n, double **C, double *c);
	void ComputeCoefficients(double **CARR, const std::vector<double> &W_X, double lambda, int m, int n, double **D);
	bool SolveEquations(double **D, double *c, int m, std::vector<double> &dx);
}
