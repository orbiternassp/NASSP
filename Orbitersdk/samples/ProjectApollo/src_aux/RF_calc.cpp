/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  RF Calculation Utility Functions

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

  See https://github.com/orbiternassp/NASSP/blob/Orbiter2016/COPYING.txt
  for more details.

  **************************************************************************/
#include "RF_calc.h"
#include "math.h"
#include "OrbiterAPI.h"


double RFCALC_rcvdPower(double xmitrPower, double xmitrGain, double rcvrGain, double frequency, double distance)
{
	double rcvdPower = 0;
	double wavelength = 0;

	rcvrGain = pow(10.0, (rcvrGain / 10.0));
	xmitrGain = pow(10.0, (xmitrGain / 10.0));

	wavelength = C0 / (frequency);

	rcvdPower = xmitrPower * xmitrGain * rcvrGain * pow((wavelength / (4 * PI * distance)), 2);

	return (10.0 * log10(1000.0 * rcvdPower));
}
