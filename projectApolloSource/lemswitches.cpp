/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: LEM-specific switches

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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.5  2006/06/18 16:43:07  dseagrav
  *	LM EPS fixes, LMP/CDR DC busses now powered thru CBs, ECA power-off bug fixed and ECA speed improvement
  *	
  *	Revision 1.4  2006/05/01 08:52:49  dseagrav
  *	LM checkpoint commit. Extended capabilities of IndicatorSwitch class to save memory, more LM ECA stuff, I forget what else changed. More work is needed yet.
  *	
  *	Revision 1.3  2006/04/23 04:15:46  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
  *	Revision 1.2  2006/04/17 18:14:27  movieman523
  *	Added flashing borders to all switches (I think).
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "LEMcomputer.h"
#include "IMU.h"

#include "sat5_lmpkd.h"

void LEMThreePosSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, sat5_lmpkd *s)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	lem = s;
}

LEMValveTalkback::LEMValveTalkback()

{
	Valve = 0;
	our_vessel = 0;
}

void LEMValveTalkback::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int vlv, sat5_lmpkd *s)

{
	IndicatorSwitch::Init(xp, yp, w, h, surf, row);

	Valve = vlv;
	our_vessel = s;
}

int LEMValveTalkback::GetState()

{
	//sprintf(oapiDebugString(),"SRCV %f STATE %d",SRC->Voltage(),our_vessel->GetValveState(Valve));
	if (our_vessel && (SRC->Voltage() > 20))
		return our_vessel->GetValveState(Valve) ? 1 : 0;

	return 0;
}

void LEMValveSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, sat5_lmpkd *s, int valve, IndicatorSwitch *ind)

{
	LEMThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row, s);

	Valve = valve;
	Indicator = ind;
}

bool LEMValveSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (LEMThreePosSwitch::CheckMouseClick(event, mx, my)) {
		CheckValve(GetState());
		return true;
	}

	return false;
}

bool LEMValveSwitch::SwitchTo(int newState)

{
	if (LEMThreePosSwitch::SwitchTo(newState)) {
		// some of these switches are spring-loaded, 
		// so we have to use newState here
		CheckValve(newState);
		return true;
	}

	return false;
}

void LEMValveSwitch::CheckValve(int s) 

{
	//sprintf(oapiDebugString(),"Switching %d",SRC->Voltage());	
	if (lem && (SRC->Voltage() > 20)) {
		if (s == THREEPOSSWITCH_UP) {
			lem->SetValveState(Valve, true);
			lem->CheckRCS();
			if (Indicator)
				*Indicator = true;
		}
		else if (s == THREEPOSSWITCH_DOWN) {
			lem->SetValveState(Valve, false);
			lem->CheckRCS();
			if (Indicator)
				*Indicator = false;
		}
	}
}

void LEMBatterySwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, sat5_lmpkd *s,
							LEM_ECA *lem_eca, int src_no)

{
	LEMThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row, s);

	eca = lem_eca;
	srcno = src_no;
}

bool LEMBatterySwitch::CheckMouseClick(int event, int mx, int my)

{
	if (LEMThreePosSwitch::CheckMouseClick(event, mx, my)) {
		switch(state){
			case THREEPOSSWITCH_UP:
				switch(srcno){
					case 1: // HV 1
						eca->input_a = 1;
						if(eca->dc_source_a_tb != NULL){
 							*eca->dc_source_a_tb = 1;
						}
						break;
					case 2: // LV 1
						eca->input_a = 2;
						if(eca->dc_source_a_tb != NULL){
 							*eca->dc_source_a_tb = 2;
						}
						break;
					case 3: // HV 2
						eca->input_b = 1;
						if(eca->dc_source_b_tb != NULL){
 							*eca->dc_source_b_tb = 1;
						}
						break;
					case 4: // LV 2
						eca->input_b = 2;
						if(eca->dc_source_b_tb != NULL){
 							*eca->dc_source_b_tb = 2;
						}
						break;
				}
				break;
			case THREEPOSSWITCH_DOWN:
				switch(srcno){
					case 1: // HV 1
					case 2: // LV 1
						if(eca->dc_source_a_tb != NULL){
							*eca->dc_source_a_tb = FALSE;
						}
						eca->input_a = 0;
						break;
					case 3: // HV 2
					case 4: // LV 2
						if(eca->dc_source_b_tb != NULL){
							*eca->dc_source_b_tb = FALSE;
						}
						eca->input_b = 0;
						break;
				}
				break;
		}

		return true;
	}
	
	return false;
}

bool LEMBatterySwitch::SwitchTo(int newState)

{
	sprintf(oapiDebugString(),"NewState %d",newState);
	if (LEMThreePosSwitch::SwitchTo(newState)) {
		// some of these switches are spring-loaded, 
		// so we have to use newState here
		// CheckValve(newState);
		return true;
	}

	return false;
}


