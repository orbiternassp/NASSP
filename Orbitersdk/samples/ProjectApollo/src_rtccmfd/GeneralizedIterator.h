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
	void GeneralizedIterator(bool(*state_evaluation)(void *, std::vector<double>, void*, std::vector<double>&), const std::vector<double> &Y_min, const std::vector<double> &Y_max, const std::vector<double> &var_guess, const std::vector<double> &stepsizes, const std::vector<double> &x_weights, void *constants, void *data, const std::vector<int> &class_des, const std::vector<double> &y_weight, std::vector<double> &x_res, std::vector<double> &y_res);
	double CalcCost(const std::vector<double> &A, const std::vector<double> &B);
	void CalcDX2(double **P, const std::vector<double> &W_X, const std::vector<double> &W_Y, double lambda, const std::vector<double> &dy, int m, int n, std::vector<double> &dx);
}
