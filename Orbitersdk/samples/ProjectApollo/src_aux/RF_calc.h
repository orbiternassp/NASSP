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

/// \brief Calcluate Received Power
///
/// This function is a simple inplimentation of Friis transmission equation.
///		More info: https://en.wikipedia.org/wiki/Friis_transmission_equation
///
/// \param xmitrPower The power of the transmitter in watts RMS
/// \param xmitrGain The gain or directivity of the transmitter in dBi
/// \param frequency The frequency of the signal being transmitted in Hz
/// \param distance  The distance between the transmitter and the receiver in meters
/// \return The power recieved by the receiver in watts RMS
double RFCALC_rcvdPower(double xmitrPower, double xmitrGain, double rcvrGain, double frequency, double distance);