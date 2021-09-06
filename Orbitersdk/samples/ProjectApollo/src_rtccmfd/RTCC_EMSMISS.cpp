/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2021

  RTCC Numerical Integration Control Element - EMSMISS

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

#include "RTCC_EMSMISS.h"
#include "rtcc.h"

RTCC_EMSMISS::RTCC_EMSMISS(RTCC *r) : RTCCModule(r)
{

}

void RTCC_EMSMISS::Call(EMSMISSInputTable &in)
{
	//Get input weights
	EMMENIInputTable emmeniin;
	PLAWDTInput plawdtin;
	PLAWDTOutput plawdtout;
	double DensityMultiplier;

	if (in.DensityMultOverrideIndicator)
	{
		DensityMultiplier = in.DensityMultiplier;
	}
	else
	{
		DensityMultiplier = 1.0;
	}

	if (in.useInputWeights)
	{
		plawdtout = *in.WeightsTable;
	}
	else
	{
		plawdtin.T_UP = in.AnchorVector.GMT;
		plawdtin.Num = in.IgnoreManueverNumber;
		plawdtin.KFactorOpt = false;
		plawdtin.TableCode = in.VehicleCode;
		plawdtin.VentingOpt = true;
		pRTCC->PLAWDT(plawdtin, plawdtout);
	}

	emmeniin.AnchorVector = in.AnchorVector;
	emmeniin.Area = plawdtout.ConfigArea;
	emmeniin.CutoffIndicator = in.CutoffIndicator;
	emmeniin.DensityMultiplier = DensityMultiplier * plawdtout.KFactor;
	emmeniin.EarthRelStopParam = in.EarthRelStopParam;
	emmeniin.EphemerisBuildIndicator = false;
	emmeniin.IsForwardIntegration = in.IsForwardIntegration;
	emmeniin.MaxIntegTime = in.MaxIntegTime;
	emmeniin.MoonRelStopParam = in.MoonRelStopParam;
	emmeniin.StopParamRefFrame = in.StopParamRefFrame;
	//TBD: Vent
	emmeniin.Weight = plawdtout.ConfigWeight;

	pRTCC->EMMENI(emmeniin);
}