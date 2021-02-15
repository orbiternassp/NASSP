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

#pragma once
#include "math.h"
#include "OrbiterAPI.h"

#define RF_ZERO_POWER_DBM -1000.0; //safe "0" for power, nothing should ever get this low

/// \brief Calcluate Received Power
///
/// This function is a simple inplimentation of Friis transmission equation.
///		More info: https://en.wikipedia.org/wiki/Friis_transmission_equation
///
/// \param xmitrPower The power of the transmitter in watts RMS
/// \param xmitrGain The gain or directivity of the transmitter in dBi
/// \param frequency The frequency of the signal being transmitted in Hz
/// \param distance  The distance between the transmitter and the receiver in meters
/// \return The power recieved by the receiver in dBm
static inline double RFCALC_rcvdPower(double xmitrPower, double xmitrGain, double rcvrGain, double frequency, double distance)
{
	double rcvdPower = 0;
	double wavelength = 0;

	rcvrGain = pow(10.0, (rcvrGain / 10.0));
	xmitrGain = pow(10.0, (xmitrGain / 10.0));

	wavelength = C0 / (frequency);

	rcvdPower = xmitrPower * xmitrGain * rcvrGain * pow((wavelength / (4 * PI * distance)), 2); //watts
	rcvdPower = 10.0 * log10(1000.0 * rcvdPower); //convert to dBm

	if (fpclassify(rcvdPower) != FP_NORMAL)
	{
		return RF_ZERO_POWER_DBM;
	}

	return rcvdPower;
}