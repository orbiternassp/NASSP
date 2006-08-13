/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: LEM systems code

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
  *	Revision 1.17  2006/08/13 16:01:52  movieman523
  *	Renamed LEM. Think it all builds properly, I'm checking it in before the lightning knocks out the power here :).
  *	
  *	Revision 1.16  2006/08/13 10:37:55  dseagrav
  *	TTCA enabled in JETS mode
  *	
  *	Revision 1.15  2006/08/13 06:30:49  dseagrav
  *	LM checkpoint commit.
  *	
  *	Revision 1.14  2006/07/24 06:41:29  dseagrav
  *	Many changes - Rearranged / corrected FDAI power usage, added LM AC equipment, many bugfixes
  *	
  *	Revision 1.13  2006/06/18 22:45:31  dseagrav
  *	LM ECA bug fix, LGC,IMU,DSKY and IMU OPR wired to CBs, IMU OPR,LGC,FDAI,and DSKY draw power
  *	
  *	Revision 1.12  2006/06/18 16:43:07  dseagrav
  *	LM EPS fixes, LMP/CDR DC busses now powered thru CBs, ECA power-off bug fixed and ECA speed improvement
  *	
  *	Revision 1.11  2006/06/11 09:20:30  dseagrav
  *	LM ECA #2 added, ECA low-voltage tap usage added, CDR & LMP DC busses wired to ECAs
  *	
  *	Revision 1.10  2006/05/01 08:52:50  dseagrav
  *	LM checkpoint commit. Extended capabilities of IndicatorSwitch class to save memory, more LM ECA stuff, I forget what else changed. More work is needed yet.
  *	
  *	Revision 1.9  2006/04/23 04:15:46  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
  *	Revision 1.8  2006/04/12 06:27:19  dseagrav
  *	LM checkpoint commit. The LM is not airworthy at this point. Please be patient.
  *	
  *	Revision 1.7  2006/01/14 21:59:52  movieman523
  *	Added PanelSDK, init, timestep, save and load.
  *	
  *	Revision 1.6  2006/01/09 19:26:03  tschachim
  *	More attempts to make code build on MS C++ 2005
  *	
  *	Revision 1.5  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.4  2005/08/10 20:00:55  spacex15
  *	Activated 3 position lem eng arm switch
  *	
  *	Revision 1.3  2005/08/09 09:19:12  tschachim
  *	Introduced toggleswitch lib
  *	
  *	Revision 1.2  2005/08/07 19:28:08  lazyd
  *	Changed LM RCS and ascent parameters to historical
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"
#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "lm_channels.h"
#include "dsky.h"
#include "IMU.h"

#include "LEM.h"

void LEM::ResetThrusters()

{
	SetAttitudeRotLevel(0, 0);
	SetAttitudeRotLevel(1, 0);
	SetAttitudeRotLevel(2, 0);
	SetAttitudeLinLevel(0, 0);
	SetAttitudeLinLevel(1, 0);
	SetAttitudeLinLevel(2, 0);
	ActivateNavmode(NAVMODE_KILLROT);
}

/* NOT USED? DS20060413
void LEM::AddRCS_LM(double TRANZ)
{
	UINT atthand;
	const double ATTCOOR = 0;
	const double ATTCOOR2 = 1.53;
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH=.2;
	const double ATTHEIGHT=.5;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;

		//lin Forward

		VECTOR3 m_exhaust_pos2= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_pos3= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_pos4= _V(ATTCOOR2+0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_pos5= _V(-ATTCOOR2-0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_ref2 = _V(0.35,-0.35,-1);
		VECTOR3 m_exhaust_ref3 = _V(-0.35,-0.35,-1);
		VECTOR3 m_exhaust_ref4 = _V(0.35,0.35,-1);
		VECTOR3 m_exhaust_ref5 = _V(-0.35,0.35,-1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);

		//lin backward

		m_exhaust_pos2= _V(ATTCOOR2+0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0,0.0,1);
		m_exhaust_ref5 = _V(0,0.0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);

		//lin Left

		m_exhaust_pos2= _V(-ATTCOOR2-0.27,-ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2-0.27,ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,1,0);
		m_exhaust_ref3 = _V(-1,-1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);

		//lin right

		m_exhaust_pos2= _V(ATTCOOR2+0.27,-ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(ATTCOOR2+0.27,ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(1,1,0);
		m_exhaust_ref3 = _V(1,-1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);

		//lin down

		m_exhaust_pos2= _V(ATTCOOR2,ATTCOOR2+0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2,ATTCOOR2+0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,1,0);
		m_exhaust_ref3 = _V(1,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		//lin up
		m_exhaust_pos2= _V(ATTCOOR2,-ATTCOOR2-0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2,-ATTCOOR2-0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,-1,0);
		m_exhaust_ref3 = _V(1,-1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);

		//Rot up

		m_exhaust_pos2= _V(ATTCOOR2+0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0.35,-0.35,-1);
		m_exhaust_ref5 = _V(-0.35,-0.35,-1);
		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);

		//Rot down
		m_exhaust_pos2= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0.35,0.35,-1);
		m_exhaust_ref5 = _V(-0.35,0.35,-1);
		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);


		//Rot left

		m_exhaust_pos2= _V(-ATTCOOR2-0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(ATTCOOR2+0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0.35,-0.35,-1);
		m_exhaust_ref5 = _V(0.35,0.35,-1);

		atthand =AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);

		//Rot right

		m_exhaust_pos2= _V(ATTCOOR2+0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(-0.35,-0.35,-1);
		m_exhaust_ref5 = _V(-0.35,0.35,-1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);


		//Roll left

		m_exhaust_pos2= _V(ATTCOOR2+0.27,ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2-0.27,-ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_pos4= _V(ATTCOOR2,-ATTCOOR2-0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(1,-1,0);
		m_exhaust_ref3 = _V(-1,1,0);
		m_exhaust_ref4 = _V(-1,-1,0);
		m_exhaust_pos5= _V(-ATTCOOR2,ATTCOOR2+0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_ref5 = _V(1,1,0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);

		//Roll right
		m_exhaust_pos2= _V(-ATTCOOR2-0.27,ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(ATTCOOR2+0.27,-ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_pos4= _V(ATTCOOR2,ATTCOOR2+0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_pos5= _V(-ATTCOOR2,-ATTCOOR2-0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,-1,0);
		m_exhaust_ref3 = _V(1,1,0);
		m_exhaust_ref4 = _V(-1,1,0);
		m_exhaust_ref5 = _V(1,-1,0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
}

void LEM::AddRCS_LM2(double TRANZ)
{
UINT atthand;
	const double ATTCOOR = 0;
	const double ATTCOOR2 = 1.53;
	const double ATTCOORY2 = 1.53+.28;
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH=.2;
	const double ATTHEIGHT=.5;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;
	const double LM2VOFS=0.28;

//	int MaxThrust=120;
//	double RCSISP=15000;
	double MaxThrust=445.0;
	double RCSISP=2840.0;

/*

	th_att_lin[0]=CreateThruster (_V(2,2,-2), _V(1,0,1),   MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[1]=CreateThruster (_V(-2,2,-2), _V(-1,0,1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[2]=CreateThruster (_V(2,-2,-2), _V(1,0,1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[3]=CreateThruster (_V(-2,-2,-2), _V(-1,0,1),MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[4]=CreateThruster (_V(2,2,2), _V(1,0,-1),   MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[5]=CreateThruster (_V(-2,2,2), _V(-1,0,-1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[6]=CreateThruster (_V(2,-2,2), _V(1,0,-1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[7]=CreateThruster (_V(-2,-2,2), _V(-1,0,-1),MaxThrust, ph_rcslm1, RCSISP, RCSISP);

	CreateThrusterGroup (th_att_lin,   4, THGROUP_ATT_FORWARD);
	CreateThrusterGroup (th_att_lin+4, 4, THGROUP_ATT_BACK);
*/

//	th_att_rot[2]=CreateThruster (_V(2,2,-2), _V(0,0,1),   MaxThrust, ph_rcslm1, RCSISP, RCSISP);
//	th_att_rot[4]=CreateThruster (_V(-2,2,-2), _V(0,0,1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
//	th_att_rot[3]=CreateThruster (_V(2,-2,-2), _V(0,0,1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
//	th_att_rot[5]=CreateThruster (_V(-2,-2,-2), _V(0,0,1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
//	th_att_rot[7]=CreateThruster (_V(2,2,2), _V(0,0,-1),   MaxThrust, ph_rcslm1, RCSISP, RCSISP);
//	th_rcs[0]=CreateThruster (_V(-2,2,2), _V(0,0,-1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
//	th_att_rot[6]=CreateThruster (_V(2,-2,2), _V(0,0,-1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
//	th_att_rot[1]=CreateThruster (_V(-2,-2,2), _V(0,0,-1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
//	CreateThrusterGroup (th_att_rot,   4, THGROUP_ATT_YAWLEFT);
//	CreateThrusterGroup (th_att_rot+4,   4, THGROUP_ATT_YAWRIGHT);
//	CreateThrusterGroup (th_rcs,   1, THGROUP_ATT_YAWLEFT);

	/*
	th_att_lin[8]=CreateThruster (_V(2,2,2), _V(-1,0,0),   MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[9]=CreateThruster (_V(2,-2,2), _V(-1,0,0),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[10]=CreateThruster (_V(2,2,-2), _V(-1,0,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[11]=CreateThruster (_V(2,-2,-2), _V(-1,0,0),MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[12]=CreateThruster (_V(-2,2,2), _V(1,0,0),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[13]=CreateThruster (_V(-2,2,-2), _V(1,0,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[14]=CreateThruster (_V(-2,-2,2), _V(1,0,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[15]=CreateThruster (_V(-2,-2,-2), _V(1,0,0),MaxThrust, ph_rcslm1, RCSISP, RCSISP);

	th_att_lin[16]=CreateThruster (_V(2,-2,2), _V(0,1,0),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[17]=CreateThruster (_V(-2,-2,2), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[18]=CreateThruster (_V(2,-2,-2), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[19]=CreateThruster (_V(-2,-2,-2), _V(0,1,0),MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[20]=CreateThruster (_V(2,2,2), _V(0,-1,0),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[21]=CreateThruster (_V(-2,2,2), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[22]=CreateThruster (_V(2,2,-2), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_lin[23]=CreateThruster (_V(-2,2,-2), _V(0,-1,0),MaxThrust, ph_rcslm1, RCSISP, RCSISP);

	CreateThrusterGroup (th_att_lin+16,   4, THGROUP_ATT_UP);
	CreateThrusterGroup (th_att_lin+20,   4, THGROUP_ATT_DOWN);
	CreateThrusterGroup (th_att_lin+12,  4 , THGROUP_ATT_RIGHT);
	CreateThrusterGroup (th_att_lin+8, 4, THGROUP_ATT_LEFT);

	th_att_rot[8]=CreateThruster (_V(2,2,-2), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[9]=CreateThruster (_V(-2,2,-2), _V(0,-1,0),MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[12]=CreateThruster (_V(2,2,2), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[13]=CreateThruster (_V(-2,2,2), _V(0,-1,0),MaxThrust, ph_rcslm1, RCSISP, RCSISP);

	th_att_rot[15]=CreateThruster (_V(2,-2,-2), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[14]=CreateThruster (_V(-2,-2,-2), _V(0,1,0),MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[10]=CreateThruster (_V(2,-2,2), _V(0,1,0),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[11]=CreateThruster (_V(-2,-2,2), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);

	CreateThrusterGroup (th_att_rot+12,   4, THGROUP_ATT_PITCHDOWN);
	CreateThrusterGroup (th_att_rot+8,   4, THGROUP_ATT_PITCHUP);

	th_att_rot[16]=CreateThruster (_V(2,2,-2), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[17]=CreateThruster (_V(2,2,2), _V(0,-1,0),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[18]=CreateThruster (_V(-2,-2,2), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[19]=CreateThruster (_V(-2,-2,-2), _V(0,1,0),MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[23]=CreateThruster (_V(-2,2,2), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[22]=CreateThruster (_V(2,-2,-2), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[20]=CreateThruster (_V(-2,2,-2), _V(0,-1,0),MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[21]=CreateThruster (_V(2,-2,2), _V(0,1,0),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);

	CreateThrusterGroup (th_att_rot+20,   4, THGROUP_ATT_BANKLEFT);
	CreateThrusterGroup (th_att_rot+16,   4, THGROUP_ATT_BANKRIGHT);

	SURFHANDLE tex = oapiRegisterExhaustTexture ("Exhaust_atrcs");//"Exhaust2"
	//for (i = 0; i < 24; i++) vessel->AddExhaust (th_att_lin[i], 1.2, 0.18,tex);
	//for (i = 0; i < 24; i++) vessel->AddExhaust (th_att_rot[i], 1.2, 0.18,tex);
	//return;

		//lin Forward

		VECTOR3 m_exhaust_pos2= _V(ATTCOOR2+0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_pos3= _V(-ATTCOOR2-0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_pos4= _V(ATTCOOR2+0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_pos5= _V(-ATTCOOR2-0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_ref2 = _V(0.35,-0.35,-1);
		VECTOR3 m_exhaust_ref3 = _V(-0.35,-0.35,-1);
		VECTOR3 m_exhaust_ref4 = _V(0.35,0.35,-1);
		VECTOR3 m_exhaust_ref5 = _V(-0.35,0.35,-1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);

		//lin backward

		m_exhaust_pos2= _V(ATTCOOR2+0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0,0.0,1);
		m_exhaust_ref5 = _V(0,0.0,1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);

		//lin Left

		m_exhaust_pos2= _V(-ATTCOOR2-0.27,-ATTCOORY2+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2-0.27,ATTCOOR2-.28+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,1,0);
		m_exhaust_ref3 = _V(-1,-1,0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);

		//lin right

		m_exhaust_pos2= _V(ATTCOOR2+0.27,-ATTCOORY2+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(ATTCOOR2+0.27,ATTCOOR2-.28+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(1,1,0);
		m_exhaust_ref3 = _V(1,-1,0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);

		//lin down

		m_exhaust_pos2= _V(ATTCOOR2,ATTCOOR2-.28+0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2,ATTCOOR2-.28+0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,1,0);
		m_exhaust_ref3 = _V(1,1,0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);

		//lin up
		m_exhaust_pos2= _V(ATTCOOR2,-ATTCOORY2-0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2,-ATTCOORY2-0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,-1,0);
		m_exhaust_ref3 = _V(1,-1,0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);

		//Rot up

		m_exhaust_pos2= _V(ATTCOOR2+0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0.35,-0.35,-1);
		m_exhaust_ref5 = _V(-0.35,-0.35,-1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);

		//Rot down
		m_exhaust_pos2= _V(ATTCOOR2+0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0.35,0.35,-1);
		m_exhaust_ref5 = _V(-0.35,0.35,-1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);

		//Rot left

		m_exhaust_pos2= _V(-ATTCOOR2-0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(ATTCOOR2+0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0.35,-0.35,-1);
		m_exhaust_ref5 = _V(0.35,0.35,-1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);

		//Rot right

		m_exhaust_pos2= _V(ATTCOOR2+0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(ATTCOOR2+0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(-ATTCOOR2-0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(-0.35,-0.35,-1);
		m_exhaust_ref5 = _V(-0.35,0.35,-1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);

		//Roll left

		m_exhaust_pos2= _V(ATTCOOR2+0.27,ATTCOOR2-.28+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2-0.27,-ATTCOORY2+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos4= _V(ATTCOOR2,-ATTCOORY2-0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(1,-1,0);
		m_exhaust_ref3 = _V(-1,1,0);
		m_exhaust_ref4 = _V(-1,-1,0);
		m_exhaust_pos5= _V(-ATTCOOR2,ATTCOOR2-.28+0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref5 = _V(1,1,0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);

		//Roll right
		m_exhaust_pos2= _V(-ATTCOOR2-0.27,ATTCOOR2-.28+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(ATTCOOR2+0.27,-ATTCOORY2+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos4= _V(ATTCOOR2,ATTCOOR2-.28+0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos5= _V(-ATTCOOR2,-ATTCOORY2-0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,-1,0);
		m_exhaust_ref3 = _V(1,1,0);
		m_exhaust_ref4 = _V(-1,1,0);
		m_exhaust_ref5 = _V(1,-1,0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		*/
/*
}
*/


void LEM::AddRCS_LMH(double TRANZ)
{
	const double ATTCOOR = 1.78;
	const double ATTCOOR2 = 1.35;
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH=.15;
	const double ATTHEIGHT=3;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;

	double MaxThrust=445.0;
	double RCSISP=2840.0;

	SURFHANDLE tex = oapiRegisterExhaustTexture ("Exhaust_atrcs");
	// A1U
	th_rcs[0]=CreateThruster(_V(-ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[0],ATTHEIGHT,ATTWIDTH,tex);
	// A1F
	th_rcs[1]=CreateThruster(_V(-ATTCOOR+.05,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10), _V(0,0,-1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[1],ATTHEIGHT,ATTWIDTH,tex);
	// B1L
	th_rcs[2]=CreateThruster(_V(-ATTCOOR-0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17), _V(1,0,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[2],ATTHEIGHT,ATTWIDTH,tex);
	// B1D
	th_rcs[3]=CreateThruster(_V(-ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[3],ATTHEIGHT,ATTWIDTH,tex);

	// B2U
	th_rcs[4]=CreateThruster(_V(-ATTCOOR+.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.30), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[4],ATTHEIGHT,ATTWIDTH,tex);
	// B2L
	th_rcs[5]=CreateThruster(_V(-ATTCOOR-0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25), _V(1,0,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[5],ATTHEIGHT,ATTWIDTH,tex);
	// A2A
	th_rcs[6]=CreateThruster(_V(-ATTCOOR+.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8), _V(0,0,1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[6],ATTHEIGHT,ATTWIDTH,tex);
	// A2D
	th_rcs[7]=CreateThruster(_V(-ATTCOOR+.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.3), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[7],ATTHEIGHT,ATTWIDTH,tex);

	// A3U
	th_rcs[8]=CreateThruster(_V(ATTCOOR-0.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.35), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[8],ATTHEIGHT,ATTWIDTH,tex);
	// A3R
	th_rcs[9]=CreateThruster(_V(ATTCOOR+0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25), _V(-1,0,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[9],ATTHEIGHT,ATTWIDTH,tex);
	// B3A
	th_rcs[10]=CreateThruster(_V(ATTCOOR-.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8), _V(0,0,1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[10],ATTHEIGHT,ATTWIDTH,tex);
	// B3D
	th_rcs[11]=CreateThruster(_V(ATTCOOR-0.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.35), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[11],ATTHEIGHT,ATTWIDTH,tex);

	// B4U
	th_rcs[12]=CreateThruster(_V(ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[12],ATTHEIGHT,ATTWIDTH,tex);
	// B4F
	th_rcs[13]=CreateThruster(_V(ATTCOOR-.001,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10), _V(0,0,-1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[13],ATTHEIGHT,ATTWIDTH,tex);
	// A4R
	th_rcs[14]=CreateThruster(_V(ATTCOOR+0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17), _V(-1,0,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[14],ATTHEIGHT,ATTWIDTH,tex);
	// A4D
	th_rcs[15]=CreateThruster(_V(ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[15],ATTHEIGHT,ATTWIDTH,tex);

	// Setup Orbiter thruster groups
	th_rcs_orbiter_rot[0] = th_rcs[0];  // A1U
	th_rcs_orbiter_rot[1] = th_rcs[12]; // B4U
	th_rcs_orbiter_rot[2] = th_rcs[11]; // B3D
	th_rcs_orbiter_rot[3] = th_rcs[7];  // A2D
	CreateThrusterGroup (th_rcs_orbiter_rot+0, 4, THGROUP_ATT_PITCHDOWN);
	th_rcs_orbiter_rot[4] = th_rcs[15]; // A4D
	th_rcs_orbiter_rot[5] = th_rcs[3];  // B1D
	th_rcs_orbiter_rot[6] = th_rcs[8];  // A3U
	th_rcs_orbiter_rot[7] = th_rcs[4];  // B2U
	CreateThrusterGroup (th_rcs_orbiter_rot+4, 4, THGROUP_ATT_PITCHUP);
	th_rcs_orbiter_rot[8] = th_rcs[0];  // A1U
	th_rcs_orbiter_rot[9] = th_rcs[4];  // B2U
	th_rcs_orbiter_rot[10]= th_rcs[11]; // B3D
	th_rcs_orbiter_rot[11]= th_rcs[15]; // A4D
	CreateThrusterGroup (th_rcs_orbiter_rot+8, 4, THGROUP_ATT_BANKLEFT);
	th_rcs_orbiter_rot[12]= th_rcs[3];  // B1D
	th_rcs_orbiter_rot[13]= th_rcs[7];  // A2D
	th_rcs_orbiter_rot[14]= th_rcs[12]; // B4U
	th_rcs_orbiter_rot[15]= th_rcs[8];  // A3U
	CreateThrusterGroup (th_rcs_orbiter_rot+12, 4,THGROUP_ATT_BANKRIGHT);
	th_rcs_orbiter_rot[16]= th_rcs[1];  // A1F
	th_rcs_orbiter_rot[17]= th_rcs[14]; // A4R
	th_rcs_orbiter_rot[18]= th_rcs[10]; // B3A
	th_rcs_orbiter_rot[19]= th_rcs[5];  // B2L
	CreateThrusterGroup (th_rcs_orbiter_rot+16, 4,THGROUP_ATT_YAWLEFT);
	th_rcs_orbiter_rot[20]= th_rcs[2];  // B1L
	th_rcs_orbiter_rot[21]= th_rcs[9];  // A3R
	th_rcs_orbiter_rot[22]= th_rcs[13]; // B4F
	th_rcs_orbiter_rot[23]= th_rcs[6];  // A2A
	CreateThrusterGroup (th_rcs_orbiter_rot+20, 4,THGROUP_ATT_YAWRIGHT);

	/* THRUSTER TABLE:
		0	A1U		8	A3U
		1	A1F		9	A3R
		2	B1L		10	B3A
		3	B1D		11	B3D

		4	B2U		12	B4U
		5	B2L		13	B4F
		6	A2A		14	A4R
		7	A2D		15	A4D
	*/

//	CreateThrusterGroup (th_rcs,   1, THGROUP_ATT_YAWLEFT);
}

void LEM::AddRCS_LMH2(double TRANZ)
{
	const double ATTCOOR = 1.78;
	const double ATTCOOR2 = -0.50;
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH=.15;
	const double ATTHEIGHT=3;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;

	double MaxThrust=445.0;
	double RCSISP=2840.0;

	SURFHANDLE tex = oapiRegisterExhaustTexture ("Exhaust_atrcs");
	// A1U
	th_rcs[0]=CreateThruster(_V(-ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[0],ATTHEIGHT,ATTWIDTH,tex);
	// A1F
	th_rcs[1]=CreateThruster(_V(-ATTCOOR+.05,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10), _V(0,0,-1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[1],ATTHEIGHT,ATTWIDTH,tex);
	// B1L
	th_rcs[2]=CreateThruster(_V(-ATTCOOR-0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17), _V(1,0,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[2],ATTHEIGHT,ATTWIDTH,tex);
	// B1D
	th_rcs[3]=CreateThruster(_V(-ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[3],ATTHEIGHT,ATTWIDTH,tex);

	// B2U
	th_rcs[4]=CreateThruster(_V(-ATTCOOR+.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.30), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[4],ATTHEIGHT,ATTWIDTH,tex);
	// B2L
	th_rcs[5]=CreateThruster(_V(-ATTCOOR-0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25), _V(1,0,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[5],ATTHEIGHT,ATTWIDTH,tex);
	// A2A
	th_rcs[6]=CreateThruster(_V(-ATTCOOR+.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8), _V(0,0,1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[6],ATTHEIGHT,ATTWIDTH,tex);
	// A2D
	th_rcs[7]=CreateThruster(_V(-ATTCOOR+.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.3), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[7],ATTHEIGHT,ATTWIDTH,tex);

	// A3U
	th_rcs[8]=CreateThruster(_V(ATTCOOR-0.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.35), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[8],ATTHEIGHT,ATTWIDTH,tex);
	// A3R
	th_rcs[9]=CreateThruster(_V(ATTCOOR+0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25), _V(-1,0,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[9],ATTHEIGHT,ATTWIDTH,tex);
	// B3A
	th_rcs[10]=CreateThruster(_V(ATTCOOR-.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8), _V(0,0,1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[10],ATTHEIGHT,ATTWIDTH,tex);
	// B3D
	th_rcs[11]=CreateThruster(_V(ATTCOOR-0.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.35), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[11],ATTHEIGHT,ATTWIDTH,tex);

	// B4U
	th_rcs[12]=CreateThruster(_V(ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[12],ATTHEIGHT,ATTWIDTH,tex);
	// B4F
	th_rcs[13]=CreateThruster(_V(ATTCOOR-.001,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10), _V(0,0,-1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[13],ATTHEIGHT,ATTWIDTH,tex);
	// A4R
	th_rcs[14]=CreateThruster(_V(ATTCOOR+0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17), _V(-1,0,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[14],ATTHEIGHT,ATTWIDTH,tex);
	// A4D
	th_rcs[15]=CreateThruster(_V(ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	AddExhaust(th_rcs[15],ATTHEIGHT,ATTWIDTH,tex);
}

/* OBSOLETED DS20060410

void LEM::SetRCS(PROPELLANT_HANDLE ph_prop)
{
		if(ATT2switch && QUAD1switch && QUAD2switch && QUAD3switch  && QUAD4switch  && QUAD5switch  && QUAD6switch  && QUAD7switch  && QUAD8switch && GetValveState(LEM_RCS_MAIN_SOV_A) && GetValveState(LEM_RCS_MAIN_SOV_B) && ED1switch && ED4switch && RCSS1switch && RCSS2switch && RCSS3switch && RCSS4switch){
			for(int i=8;i<16;i++){
				SetThrusterResource(th_att_rot[i],ph_prop);
				SetThrusterResource(th_att_lin[i+8],ph_prop);
			}
		}else{
			for(int i=8;i<16;i++){
				SetThrusterResource(th_att_rot[i],NULL);
				SetThrusterResource(th_att_lin[i+8],NULL);
			}
		}
		if(ATT3switch && QUAD1switch && QUAD2switch && QUAD3switch  && QUAD4switch  && QUAD5switch  && QUAD6switch  && QUAD7switch  && QUAD8switch && GetValveState(LEM_RCS_MAIN_SOV_A) && GetValveState(LEM_RCS_MAIN_SOV_B) && ED1switch && ED4switch && RCSS1switch && RCSS2switch && RCSS3switch && RCSS4switch){
			for(int i=0;i<8;i++){
				SetThrusterResource(th_att_rot[i],ph_prop);
				SetThrusterResource(th_att_lin[i+8],ph_prop);
			}
		}else{
			for(int i=0;i<8;i++){
				SetThrusterResource(th_att_rot[i],NULL);
				SetThrusterResource(th_att_lin[i+8],NULL);
			}
		}
		if(ATT1switch && QUAD1switch && QUAD2switch && QUAD3switch  && QUAD4switch  && QUAD5switch  && QUAD6switch  && QUAD7switch  && QUAD8switch && GetValveState(LEM_RCS_MAIN_SOV_A) && GetValveState(LEM_RCS_MAIN_SOV_B) && ED1switch && ED4switch && RCSS1switch && RCSS2switch && RCSS3switch && RCSS4switch){
			for(int i=0;i<8;i++){
				SetThrusterResource(th_att_rot[i+16],ph_prop);
				SetThrusterResource(th_att_lin[i],ph_prop);
			}
		}else{
			for(int i=0;i<8;i++){
				SetThrusterResource(th_att_rot[i+16],NULL);
				SetThrusterResource(th_att_lin[i],NULL);
			}
		}
	return;
} */

bool LEM::CabinFansActive()

{
	return CABFswitch;
}

bool LEM::AscentEngineArmed()

{
	return (EngineArmSwitch.IsUp()); //&& !ASCHE1switch && !ASCHE2switch && ED1switch && ED6switch && ED7switch && ED8switch;
}

bool LEM::AscentRCSArmed()

{
	return AFEED1switch || AFEED2switch || AFEED3switch || AFEED4switch;
}

// DS20060302 DX8 callback for enumerating joysticks
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pLEM)
{
	class LEM * lem = (LEM*)pLEM; // Pointer to us
	HRESULT hr;

	if(lem->js_enabled > 1){  // Do we already have enough joysticks?
		return DIENUM_STOP; } // If so, stop enumerating additional devices.

	// Obtain an interface to the enumerated joystick.
    hr = lem->dx8ppv->CreateDevice(pdidInstance->guidInstance, &lem->dx8_joystick[lem->js_enabled], NULL);
	
	if(FAILED(hr)) {              // Did that work?
		return DIENUM_CONTINUE; } // No, keep enumerating (if there's more)

	lem->js_enabled++;      // Otherwise, Next!
	return DIENUM_CONTINUE; // and keep enumerating
}

void LEM::SystemsInit()

{
	Panelsdk.RegisterVessel(this);
	Panelsdk.InitFromFile("ProjectApollo/LEMSystems");

	// DS20060407 Start wiring things together

	// Batteries
	Battery1 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_A");
	Battery2 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_B");
	Battery3 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_C");
	Battery4 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_D");
	Battery5 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:ASC_BATTERY_A");
	Battery6 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:ASC_BATTERY_B");
	LunarBattery = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:LUNAR_BATTERY");

	// ECA #1 (DESCENT stage, LMP DC bus)
	ECA_1.dc_source_hi_a = Battery1;
	ECA_1.dc_source_lo_a = Battery1;
	ECA_1.dc_source_hi_b = Battery2;
	ECA_1.dc_source_lo_b = Battery2;
	ECA_1.dc_source_a_tb = &DSCBattery1TB;
	*ECA_1.dc_source_a_tb = FALSE; // Initialize to off
	ECA_1.dc_source_b_tb = &DSCBattery2TB;
	*ECA_1.dc_source_b_tb = FALSE;

	// ECA #2 (DESCENT stage, CDR DC bus)
	ECA_2.dc_source_hi_a = Battery3;
	ECA_2.dc_source_lo_a = Battery3;
	ECA_2.dc_source_hi_b = Battery4;
	ECA_2.dc_source_lo_b = Battery4;
	ECA_2.dc_source_a_tb = &DSCBattery3TB;
	*ECA_2.dc_source_a_tb = FALSE; 
	ECA_2.dc_source_b_tb = &DSCBattery4TB;
	*ECA_2.dc_source_b_tb = FALSE;

	// Descent battery TBs
	DSCBattery1TB.WireTo(&ECA_1);
	DSCBattery2TB.WireTo(&ECA_1);
	DSCBattery3TB.WireTo(&ECA_2);
	DSCBattery4TB.WireTo(&ECA_2);

	// ECA Output CBs
	LMPBatteryFeedTieCB2.MaxAmps = 100.0;
	LMPBatteryFeedTieCB2.WireTo(&ECA_1);
	CDRBatteryFeedTieCB2.MaxAmps = 100.0;
	CDRBatteryFeedTieCB2.WireTo(&ECA_2);

	// CDR and LMP 28V DC busses.
	// Apparently unpowered (Wired to NULL) busses get 28V for some reason...
	CDRs28VBus.WireTo(&CDRBatteryFeedTieCB2); 
	LMPs28VBus.WireTo(&LMPBatteryFeedTieCB2);

	// AC Inverter CBs
	CDRInverter1CB.MaxAmps = 30.0;
	CDRInverter1CB.WireTo(&CDRs28VBus);
	// AC Inverters
	INV_1.dc_input = &CDRInverter1CB;
	INV_2.dc_input = &LMPInverter2CB; 
	// AC bus input breakers
	AC_A_INV_1_FEED_CB.MaxAmps = 5.0;
	AC_A_INV_1_FEED_CB.WireTo(&INV_1);
	AC_B_INV_1_FEED_CB.MaxAmps = 5.0;
	AC_B_INV_1_FEED_CB.WireTo(&INV_1);
	AC_A_INV_2_FEED_CB.MaxAmps = 5.0;
	AC_A_INV_2_FEED_CB.WireTo(&INV_2);
	AC_B_INV_2_FEED_CB.MaxAmps = 5.0;
	AC_B_INV_2_FEED_CB.WireTo(&INV_2);

	// AC busses
	ACBusA.Volts = 0;
	ACBusA.WireTo(NULL);
	ACBusB.Volts = 0;
	ACBusB.WireTo(NULL);
	// Situation load will wire these to their breakers later if needed

	// RCS Main Shutoff valves
	RCSMainSovASwitch.WireTo(&CDRs28VBus);
	RCSMainSovATB.WireTo(&CDRs28VBus);
	RCSMainSovBTB.WireTo(&LMPs28VBus);
	RCSMainSovBSwitch.WireTo(&LMPs28VBus);

	// LGC and DSKY
	LGC_DSKY_CB.MaxAmps = 7.5;
	LGC_DSKY_CB.WireTo(&CDRs28VBus);
	agc.WirePower(&LGC_DSKY_CB,&LGC_DSKY_CB);
	dsky.Init(&LGC_DSKY_CB);

	// IMU OPERATE power (Logic DC power)
	IMU_OPR_CB.MaxAmps = 20.0;
	IMU_OPR_CB.WireTo(&CDRs28VBus);
	imu.WireToBuses(&IMU_OPR_CB, &IMU_OPR_CB);
	// The IMU heater should be wired to something as well, but I'm not sure how it works

	// The FDAI has two CBs, AC and DC, and both are 2 amp CBs
	// CDR FDAI
	CDR_FDAI_DC_CB.MaxAmps = 2.0;
	CDR_FDAI_DC_CB.WireTo(&CDRs28VBus);
	CDR_FDAI_AC_CB.MaxAmps = 2.0;
	CDR_FDAI_AC_CB.WireTo(&ACBusA);
	// And the CDR FDAI itself	
	fdaiLeft.WireTo(&CDR_FDAI_DC_CB,&CDR_FDAI_AC_CB);

	//
	// HACK:
	// Not sure where these should be wired to.
	MissionTimerDisplay.WireTo(&ECA_1);
	EventTimerDisplay.WireTo(&ECA_1);

	// Arrange for updates
	Panelsdk.AddElectrical(&ECA_1, false);
	Panelsdk.AddElectrical(&ECA_2, false);
	
	Panelsdk.AddElectrical(&CDRs28VBus, false);
	Panelsdk.AddElectrical(&LMPs28VBus, false); 

	Panelsdk.AddElectrical(&INV_1, false);
	Panelsdk.AddElectrical(&INV_2, false);

	// DS20060413 Initialize joystick
	HRESULT         hr;
	js_enabled = 0;  // Disabled
	rhc_id = -1;     // Disabled
	rhc_rot_id = -1; // Disabled
	rhc_sld_id = -1; // Disabled
	rhc_rzx_id = -1; // Disabled
	thc_id = -1;     // Disabled
	thc_rot_id = -1; // Disabled
	thc_sld_id = -1; // Disabled
	thc_rzx_id = -1; // Disabled
	thc_debug = -1;
	rhc_debug = -1;
	FILE *fd;
	// Open configuration file
	fd = fopen("Config\\ProjectApollo\\Joystick.INI","r");
	if(fd != NULL){ // Did that work?
		char dataline[256];
		char *token;
		char *parameter;
		rhc_id = 0; // Trap!
		while(!feof(fd)){
			fgets(dataline,256,fd); // Yes, so read a line
			// Get a token.
			token = strtok(dataline," \r\n");
			if(token != NULL){                                  // If it's not null, parse.
				if(strncmp(token,"RHC",3)==0){                  // RHC address?
					// Get next token, which should be JS number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						rhc_id = atoi(parameter);
						if(rhc_id > 1){ rhc_id = 1; } // Be paranoid
					}
				}
				if(strncmp(token,"RRT",3)==0){                  // RHC ROTATOR address?
					// Get next token, which should be ROTATOR number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						rhc_rot_id = atoi(parameter);
						if(rhc_rot_id > 2){ rhc_rot_id = 2; } // Be paranoid
					}
				}
				if(strncmp(token,"RSL",3)==0){                  // RHC SLIDER address?
					// Get next token, which should be SLIDER number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						rhc_sld_id = atoi(parameter);
						if(rhc_sld_id > 2){ rhc_sld_id = 2; } // Be paranoid
					}
				}
				if(strncmp(token,"RZX",3)==0){                  // RHC ROTATOR address?
					// Get next token, which should be ROTATOR number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						rhc_rzx_id = atoi(parameter);
						if(rhc_rzx_id > 1){ rhc_rzx_id = 1; } // Be paranoid
					}
				}
				/* *** THC *** */
				if(strncmp(token,"THC",3)==0){                  // THC address?
					// Get next token, which should be JS number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						thc_id = atoi(parameter);
						if(thc_id > 1){ thc_id = 1; } // Be paranoid
					}
				}
				if(strncmp(token,"TRT",3)==0){                  // THC ROTATOR address?
					// Get next token, which should be ROTATOR number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						thc_rot_id = atoi(parameter);
						if(thc_rot_id > 2){ thc_rot_id = 2; } // Be paranoid
					}
				}
				if(strncmp(token,"TSL",3)==0){                  // THC SLIDER address?
					// Get next token, which should be SLIDER number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						thc_sld_id = atoi(parameter);
						if(thc_sld_id > 2){ thc_sld_id = 2; } // Be paranoid
					}
				}
				if(strncmp(token,"TZX",3)==0){                  // THC ROTATOR address?
					// Get next token, which should be ROTATOR number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						thc_rzx_id = atoi(parameter);
						if(thc_rzx_id > 1){ thc_rzx_id = 1; } // Be paranoid
					}
				}
				if(strncmp(token,"RDB",3)==0){					// RHC debug					
					rhc_debug = 1;
				}
				if(strncmp(token,"TDB",3)==0){					// THC debug					
					thc_debug = 1;
				}
			}			
		}		
		fclose(fd);
		// Having read the configuration file, set up DirectX...	
		hr = DirectInput8Create(dllhandle,DIRECTINPUT_VERSION,IID_IDirectInput8,(void **)&dx8ppv,NULL); // Give us a DirectInput context
		if(!FAILED(hr)){
			int x=0;
			// Enumerate attached joysticks until we find 2 or run out.
			dx8ppv->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
			if(js_enabled == 0){   // Did we get anything?			
				dx8ppv->Release(); // No. Close down DirectInput
				dx8ppv = NULL;     // otherwise it won't get closed later
				sprintf(oapiDebugString(),"DX8JS: No joysticks found");
			}else{
				while(x < js_enabled){                                // For each joystick
					dx8_joystick[x]->SetDataFormat(&c_dfDIJoystick2); // Use DIJOYSTATE2 structure to report data
					dx8_jscaps[x].dwSize = sizeof(dx8_jscaps[x]);     // Initialize size of capabilities data structure
					dx8_joystick[x]->GetCapabilities(&dx8_jscaps[x]); // Get capabilities
					x++;                                              // Next!
				}
			}
		}else{
			// We can't print an error message this early in initialization, so save this reason for later investigation.
			dx8_failure = hr;
		}
	}
	// Initialize other systems
	atca.Init(this);
}

void LEM::SystemsTimestep(double simt, double simdt) 

{
	// Zero ACA and TTCA bits in channel 31
	LMChannelValue31 val31;
	val31.Value = agc.GetInputChannel(031);
	val31.Value &= 030000; // Leaves AttitudeHold and AutomaticStab alone

	// Joystick read
	if(js_enabled > 0 && oapiGetFocusInterface() == this){		
		if(thc_id != -1 && !(thc_id < js_enabled)){
			sprintf(oapiDebugString(),"DX8JS: Joystick selected as THC does not exist.");
		}
		if(rhc_id != -1 && !(rhc_id < js_enabled)){
			sprintf(oapiDebugString(),"DX8JS: Joystick selected as RHC does not exist.");
		}
		/* ACA OPERATION:

			The LM ACA is a lot different from the CM RHC.
			The ACA works on a D/A converter.
			The OUT OF DETENT switch closes at .5 degrees of travel, and enables the proportional
			voltage circuit ("A" CIRCUIT) to operate.
			The hand controller must be moved 2 degrees to generate a count.
			8 degrees of usable travel, at .190 degrees per count.			
			10 degrees total travel = 42 counts.

		*/
		// Axes have 32768 points of travel for the 13 degrees to hard stop
		// 2520 points per degree. It breaks out of detent at .5 degres, or 1260 pulses.
		// 480 points per count.
		rhc_pos[0] = 0; // Initialize
		rhc_pos[1] = 0;
		rhc_pos[2] = 0;

		// Read data
		HRESULT hr;
		// Handle RHC
		if(rhc_id != -1 && dx8_joystick[rhc_id] != NULL){
			// CHECK FOR POWER HERE
			hr=dx8_joystick[rhc_id]->Poll();
			if(FAILED(hr)){ // Did that work?
				// Attempt to acquire the device
				hr = dx8_joystick[rhc_id]->Acquire();
				if(FAILED(hr)){
					sprintf(oapiDebugString(),"DX8JS: Cannot aquire RHC");
				}else{
					hr=dx8_joystick[rhc_id]->Poll();
				}
			}		
			dx8_joystick[rhc_id]->GetDeviceState(sizeof(dx8_jstate[rhc_id]),&dx8_jstate[rhc_id]);
			// Z-axis read.
			int rhc_rot_pos = 32768; // Initialize to centered
			if(rhc_rot_id != -1){ // If this is a rotator-type axis
				switch(rhc_rot_id){
					case 0:
						rhc_rot_pos = dx8_jstate[rhc_id].lRx; break;
					case 1:
						rhc_rot_pos = dx8_jstate[rhc_id].lRy; break;
					case 2:
						rhc_rot_pos = dx8_jstate[rhc_id].lRz; break;
				}
			}
			if(rhc_sld_id != -1){ // If this is a slider
				rhc_rot_pos = dx8_jstate[rhc_id].rglSlider[rhc_sld_id];
			}
			if(rhc_rzx_id != -1){ // If we use the native Z-axis
				rhc_rot_pos = dx8_jstate[rhc_id].lZ;
			}	
			if(dx8_jstate[rhc_id].lX > 34028){ // Out of detent RIGHT
				val31.Bits.ACAOutOfDetent = 1;
				val31.Bits.PlusAzimuth = 1;
				rhc_pos[0] = dx8_jstate[rhc_id].lX-34028; // Results are 0 - 31507
			}
			if(dx8_jstate[rhc_id].lX < 31508){ // Out of detent LEFT
				val31.Bits.ACAOutOfDetent = 1;
				val31.Bits.MinusAzimuth = 1;
				rhc_pos[0] = dx8_jstate[rhc_id].lX-31508; // Results are 0 - -31508
			}
			if(dx8_jstate[rhc_id].lY > 34028){ // Out of detent UP
				val31.Bits.ACAOutOfDetent = 1;
				val31.Bits.PlusElevation = 1;
				rhc_pos[1] = dx8_jstate[rhc_id].lY-34028; // Results are 0 - 31507
			}
			if(dx8_jstate[rhc_id].lY < 31508){ // Out of detent DOWN
				val31.Bits.ACAOutOfDetent = 1;
				val31.Bits.MinusElevation = 1;
				rhc_pos[1] = dx8_jstate[rhc_id].lY-31508; // Results are 0 - -31508
			}
			// YAW IS REVERSED
			if(rhc_rot_pos > 34028){ // Out of detent RIGHT
				val31.Bits.ACAOutOfDetent = 1;
				val31.Bits.PlusYaw = 1;
				rhc_pos[2] = 34028-rhc_rot_pos; // Results are 0 - 31507
			}
			if(rhc_rot_pos < 31508){ // Out of detent LEFT
				val31.Bits.ACAOutOfDetent = 1;
				val31.Bits.MinusYaw = 1;
				rhc_pos[2] = 31508-rhc_rot_pos; // Results are 0 - -31508
			}
		}else{
			// No JS
		}
		// sprintf(oapiDebugString(),"RHC: POS %d = %d",rhc_rot_pos,rhc_pos[2]);
		// And now the THC...
		if(thc_id != -1 && thc_id < js_enabled){
			// CHECK FOR POWER HERE
			int thc_voltage = 5; // HAX
			hr=dx8_joystick[thc_id]->Poll();
			if(FAILED(hr)){ // Did that work?
				// Attempt to acquire the device
				hr = dx8_joystick[thc_id]->Acquire();
				if(FAILED(hr)){
					sprintf(oapiDebugString(),"DX8JS: Cannot aquire THC");
				}else{
					hr=dx8_joystick[thc_id]->Poll();
				}
			}		
			// Read data
			dx8_joystick[thc_id]->GetDeviceState(sizeof(dx8_jstate[thc_id]),&dx8_jstate[thc_id]);
			// The LM TTCA is even wierder than the CM THC...			
			if(thc_voltage > 0){
				if(dx8_jstate[thc_id].lX < 16384){												
					val31.Bits.MinusY = 1;
				}
				if(dx8_jstate[thc_id].lY < 16384){
					// JETS MODE
					val31.Bits.PlusX = 1;
				}
				if(dx8_jstate[thc_id].lX > 49152){
					val31.Bits.PlusY = 1;
				}
				if(dx8_jstate[thc_id].lY > 49152){
					// JETS MODE
					val31.Bits.MinusX = 1;
				}
				// Z-axis read.
				int thc_rot_pos = 32768; // Initialize to centered
				if(thc_rot_id != -1){ // If this is a rotator-type axis
					switch(thc_rot_id){
						case 0:
							thc_rot_pos = dx8_jstate[thc_id].lRx; break;
						case 1:
							thc_rot_pos = dx8_jstate[thc_id].lRy; break;
						case 2:
							thc_rot_pos = dx8_jstate[thc_id].lRz; break;
					}
				}
				if(thc_sld_id != -1){ // If this is a slider
					thc_rot_pos = dx8_jstate[thc_id].rglSlider[thc_sld_id];
				}
				if(thc_rzx_id != -1){ // If we use the native Z-axis
					thc_rot_pos = dx8_jstate[thc_id].lZ;
				}
				if(thc_rot_pos < 16384){
					val31.Bits.MinusZ = 1;
				}
				if(thc_rot_pos > 49152){
					val31.Bits.PlusZ = 1;
				}
				if(thc_debug != -1){ sprintf(oapiDebugString(),"THC: X/Y/Z = %d / %d / %d",dx8_jstate[thc_id].lX,dx8_jstate[thc_id].lY,
					thc_rot_pos); }
			}else{
				// No JS
			}
		}
	}
	// Write back channel data
	agc.SetInputChannel(031,val31.Value);

	// Each timestep is passed to the SPSDK
	// to perform internal computations on the 
	// systems.
	Panelsdk.Timestep(simt);

	// After that come all other systems simesteps	
	agc.Timestep(MissionTime, simdt);						// Do work
	agc.SystemTimestep(simdt);								// Draw power
	dsky.Timestep(MissionTime);								// Do work
	//dsky.SystemTimestep(simdt);						    // DSKY power draw is broken.
	imu.Timestep(MissionTime);								// Do work
	imu.SystemTimestep(simdt);								// Draw power
	// Allow ATCA to operate between the FDAI and AGC/AEA so that any changes the FDAI makes
	// can be shown on the FDAI, but any changes the AGC/AEA make are visible to the ATCA.
	atca.Timestep(simdt);								// Do systems work
	fdaiLeft.Timestep(MissionTime, simdt);					// Do Work
	fdaiLeft.SystemTimestep(simdt);							// Draw Power
	MissionTimerDisplay.Timestep(MissionTime, simdt);       // These just do work
	EventTimerDisplay.Timestep(MissionTime, simdt);


	// Debug tests would go here
	/*
	double CDRAmps=0,LMPAmps=0;
	double CDRVolts = CDRs28VBus.Voltage(),LMPVolts = LMPs28VBus.Voltage();
	if(LMPVolts > 0){ LMPAmps = LMPs28VBus.PowerLoad()/LMPVolts; }
	if(CDRVolts > 0){ CDRAmps = CDRs28VBus.PowerLoad()/CDRVolts; }	
	sprintf(oapiDebugString(),"LM: LMP %f V/%f A CDR %f V/%f A | AC-A %f V AC-B %f V",LMPVolts,LMPAmps,
		CDRVolts,CDRAmps,ACBusA.Voltage(), ACBusB.Voltage());
	*/	
}

// PANEL SDK SUPPORT
void LEM::SetValveState(int valve, bool open)

{
	ValveState[valve] = open;

	int valve_state = open ? SP_VALVE_OPEN : SP_VALVE_CLOSE;

	if (pLEMValves[valve])
		*pLEMValves[valve] = valve_state;

	/*
	CheckRCSState();
	*/
}

bool LEM::GetValveState(int valve)

{
	//
	// First check whether the valve still exists!
	//

	/*
	if (valve < CM_VALVES_START) {
		if (stage > CSM_LEM_STAGE)
			return false;
	}
	*/

	if (pLEMValves[valve])
		return (*pLEMValves[valve] == SP_VALVE_OPEN);

	return ValveState[valve];
}

// SYSTEMS COMPONENTS

// ELECTRICAL CONTROL ASSEMBLY

LEM_ECA::LEM_ECA(){
	lem = NULL;
	dc_source_a_tb = NULL;
	dc_source_b_tb = NULL;
	dc_source_c_tb = NULL;
}

void LEM_ECA::Init(LEM *s,e_object *hi_a,e_object *hi_b,e_object *lo_a,e_object *lo_b){
	lem = s;
	input_a = 0;
	input_b = 0;
	dc_source_hi_a = hi_a;
	dc_source_hi_b = hi_b;
	dc_source_lo_a = lo_a;
	dc_source_lo_b = lo_b;
}

void LEM_ECA::SaveState(FILEHANDLE scn, char *start_str, char *end_str)

{
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_float(scn, "INPUT_A", input_a);
	oapiWriteScenario_float(scn, "INPUT_B", input_b);
	oapiWriteLine(scn, end_str);
}

void LEM_ECA::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	int dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp (line, "INPUT_A", 7)) {
			sscanf(line + 7, "%d", &dec);
			input_a = dec;
		}
		if (!strnicmp (line, "INPUT_B", 7)) {
			sscanf(line + 7, "%d", &dec);
			input_b = dec;
		}
	}
}

void LEM_ECA::DrawPower(double watts)

{ 
	power_load += watts;
};

void LEM_ECA::UpdateFlow(double dt){

	// ECA INPUTS CAN BE PARALLELED, BUT NOT IN THE SAME CHANNEL
	// That is, Battery 1 and 2 can be on at the same time, 
	//sprintf(oapiDebugString(),"ECA Input = %d Voltage %f Load %f",input,Volts,power_load);
	// Draw power from the source, and retake voltage, etc.

	int csrc=0;                             // Current Sources Operational
	double PowerDrawPerSource;              // Current to draw, per source
	
	// Find active sources
	switch(input_a){
		case 1:
			if(dc_source_hi_a != NULL && dc_source_hi_a->Voltage() > 0){
				csrc++;
			}
			break;
		case 2:
			if(dc_source_lo_a != NULL && dc_source_lo_a->Voltage() > 0){
				csrc++;
			}
			break;
	}
	switch(input_b){
		case 1:
			if(dc_source_hi_b != NULL && dc_source_hi_b->Voltage() > 0){
				csrc++;
			}
			break;
		case 2:
			if(dc_source_lo_b != NULL && dc_source_lo_b->Voltage() > 0){
				csrc++;
			}
			break;
	}
	// Compute draw
	if(csrc > 1){
		PowerDrawPerSource = power_load/2;
	}else{
		PowerDrawPerSource = power_load;
	}

	// Now take power
	switch(input_a){
		case 1:
			if(dc_source_hi_a != NULL){
				dc_source_hi_a->DrawPower(PowerDrawPerSource); // Draw 1:1
			}
			break;
		case 2:
			if(dc_source_lo_a != NULL){
				// Draw low
				dc_source_hi_a->DrawPower(PowerDrawPerSource*1.06); // Draw 6% more
			}
			break;
	}
	switch(input_b){
		case 1:
			if(dc_source_hi_b != NULL){
				dc_source_hi_b->DrawPower(PowerDrawPerSource); // Draw 1:1
			}
			break;
		case 2:
			if(dc_source_lo_b != NULL){
				dc_source_hi_b->DrawPower(PowerDrawPerSource*1.06); // Draw 6% more
			}
			break;
	}
	
	double A_Volts = 0;
	double A_Amperes = 0;
	double B_Volts = 0;
	double B_Amperes = 0;

	// Resupply from source
	switch(input_a){
		case 0: // NULL
			A_Volts = 0;
			A_Amperes = 0;
			break;
		case 1: // HV 1
			if(dc_source_hi_a != NULL){
				A_Volts =   dc_source_hi_a->Voltage();
				A_Amperes = dc_source_hi_a->Current();
			}
			break;
		case 2: // LV 1
			if(dc_source_hi_a != NULL){
				A_Volts =   (dc_source_hi_a->Voltage()*0.93);
				A_Amperes = dc_source_hi_a->Current();
			}
			break;
	}
	switch(input_b){
		case 0: // NULL
			B_Volts = 0;
			B_Amperes = 0;
			break;
		case 1: // HV 2
			if(dc_source_hi_b != NULL){
				B_Volts = dc_source_hi_b->Voltage();
				B_Amperes = dc_source_hi_b->Current();
			}
			break;
		case 2: // LV 2
			if(dc_source_hi_b != NULL){
				B_Volts = (dc_source_hi_b->Voltage()*0.93);
				B_Amperes = dc_source_hi_b->Current();
			}
			break;
	}
	// Final output
	switch(csrc){
		case 2: // DUAL
			Volts = (A_Volts + B_Volts) / 2;
			Amperes = A_Amperes+B_Amperes;
			break;
		case 1: // SINGLE
			if(input_a != 0){ // Only one (or no) input
				Volts = A_Volts;
				Amperes = A_Amperes;
			}else{
				Volts = B_Volts;
				Amperes = B_Amperes;
			}
			break;
		default: // OFF OR OTHER
			Volts = 0;
			Amperes = 0;
			break;
	}

	// Reset for next pass.
	e_object::UpdateFlow(dt);
	
	//sprintf(oapiDebugString(),"LM_ECA: = Inputs %d %d Voltages %f %f | Load %f Output %f V",input_a,input_b,A_Volts,B_Volts,power_load,Volts);
}

// AC INVERTER

LEM_INV::LEM_INV(){
	lem = NULL;
	active = 0;
	dc_input = NULL;
}

void LEM_INV::Init(LEM *s){
	lem = s;
}

void LEM_INV::DrawPower(double watts)

{ 
	power_load += watts;
};

void LEM_INV::UpdateFlow(double dt){

	// Reset these before pass
	Volts = 0;
	Amperes = 0;

	// If not active, die.
	if(!active){ return; }

	if(dc_input != NULL){
		// First take power from source
		dc_input->DrawPower(power_load*2.5);  // Add inefficiency
		// Then supply the bus
		if(dc_input->Voltage() > 24){		  // Above 24V input
			Volts = 115.0;                    // Regulator supplies 115V
		}else{                                // Otherwise
			Volts = dc_input->Voltage()*4.8;  // Falls out of regulation
		}                                     // until the load trips the CB
		Amperes = power_load/Volts;           // AC load amps
	}

	// Debug
	/*
	if(dc_input->Voltage() > 0){
		sprintf(oapiDebugString(),"INV: DC V = %f A = %f | AC LOAD = %f V = %f A = %f",
			dc_input->Voltage(),(power_load/dc_input->Voltage()*2.5),power_load,Volts,Amperes);
	}else{
		sprintf(oapiDebugString(),"INV: INPUT V = %f LOAD = %f",dc_input->Voltage(),power_load);
	}
	*/
	// Reset for next pass
	e_object::UpdateFlow(dt);
}


void LEM::CheckRCS()
{
	/* THRUSTER TABLE:
		0	A1U		8	A3U
		1	A1F		9	A3R
		2	B1L		10	B3A
		3	B1D		11	B3D

		4	B2U		12	B4U
		5	B2L		13	B4F
		6	A2A		14	A4R
		7	A2D		15	A4D
	*/

	/*
	sprintf(oapiDebugString(),"CheckRCS: %d %d %f %f",GetValveState(LEM_RCS_MAIN_SOV_A),GetValveState(LEM_RCS_MAIN_SOV_B),
		GetPropellantMass(ph_DscRCSA),GetPropellantMass(ph_DscRCSB)); */

	if(GetValveState(LEM_RCS_MAIN_SOV_A)){
		SetThrusterResource(th_rcs[0],ph_DscRCSA);
		SetThrusterResource(th_rcs[1],ph_DscRCSA);
		SetThrusterResource(th_rcs[6],ph_DscRCSA);
		SetThrusterResource(th_rcs[7],ph_DscRCSA);
		SetThrusterResource(th_rcs[8],ph_DscRCSA);
		SetThrusterResource(th_rcs[9],ph_DscRCSA);
		SetThrusterResource(th_rcs[14],ph_DscRCSA);
		SetThrusterResource(th_rcs[15],ph_DscRCSA);
	}else{
		SetThrusterResource(th_rcs[0],NULL);
		SetThrusterResource(th_rcs[1],NULL);
		SetThrusterResource(th_rcs[6],NULL);
		SetThrusterResource(th_rcs[7],NULL);
		SetThrusterResource(th_rcs[8],NULL);
		SetThrusterResource(th_rcs[9],NULL);
		SetThrusterResource(th_rcs[14],NULL);
		SetThrusterResource(th_rcs[15],NULL);
	}
	if(GetValveState(LEM_RCS_MAIN_SOV_B)){
		SetThrusterResource(th_rcs[2],ph_DscRCSB);
		SetThrusterResource(th_rcs[3],ph_DscRCSB);
		SetThrusterResource(th_rcs[4],ph_DscRCSB);
		SetThrusterResource(th_rcs[5],ph_DscRCSB);
		SetThrusterResource(th_rcs[10],ph_DscRCSB);
		SetThrusterResource(th_rcs[11],ph_DscRCSB);
		SetThrusterResource(th_rcs[12],ph_DscRCSB);
		SetThrusterResource(th_rcs[13],ph_DscRCSB);
	}else{
		SetThrusterResource(th_rcs[2],NULL);
		SetThrusterResource(th_rcs[3],NULL);
		SetThrusterResource(th_rcs[4],NULL);
		SetThrusterResource(th_rcs[5],NULL);
		SetThrusterResource(th_rcs[10],NULL);
		SetThrusterResource(th_rcs[11],NULL);
		SetThrusterResource(th_rcs[12],NULL);
		SetThrusterResource(th_rcs[13],NULL);
	}
	return;
}
