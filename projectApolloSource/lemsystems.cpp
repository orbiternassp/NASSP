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
#include "dsky.h"
#include "IMU.h"

#include "landervessel.h"
#include "sat5_lmpkd.h"

void sat5_lmpkd::ResetThrusters()

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
void sat5_lmpkd::AddRCS_LM(double TRANZ)
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

void sat5_lmpkd::AddRCS_LM2(double TRANZ)
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


void sat5_lmpkd::AddRCS_LMH(double TRANZ)
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

void sat5_lmpkd::AddRCS_LMH2(double TRANZ)
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

void sat5_lmpkd::SetRCS(PROPELLANT_HANDLE ph_prop)
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

bool sat5_lmpkd::CabinFansActive()

{
	return CABFswitch;
}

bool sat5_lmpkd::AscentEngineArmed()

{
	return (EngineArmSwitch.IsUp()); //&& !ASCHE1switch && !ASCHE2switch && ED1switch && ED6switch && ED7switch && ED8switch;
}

bool sat5_lmpkd::AscentRCSArmed()

{
	return AFEED1switch || AFEED2switch || AFEED3switch || AFEED4switch;
}

// DS20060302 DX8 callback for enumerating joysticks
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pLEM)
{
	class sat5_lmpkd * lem = (sat5_lmpkd*)pLEM; // Pointer to us
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

void sat5_lmpkd::SystemsInit()

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

	// ECA #1 -- AP11 LM does not have lunar battery!
	ECA_1.dc_source_hi_a = Battery1;
	ECA_1.dc_source_lo_a = Battery1;
	ECA_1.dc_source_hi_b = Battery2;
	ECA_1.dc_source_lo_b = Battery2;
	ECA_1.dc_source_a_tb = &DSCBattery1TB;
	*ECA_1.dc_source_a_tb = FALSE; // Initialize to off
	ECA_1.dc_source_b_tb = &DSCBattery2TB;
	*ECA_1.dc_source_b_tb = FALSE;

	// ECA #2
	ECA_2.dc_source_hi_a = Battery3;
	ECA_2.dc_source_lo_a = Battery3;
	ECA_2.dc_source_hi_b = Battery4;
	ECA_2.dc_source_lo_b = Battery4;
	ECA_2.dc_source_a_tb = &DSCBattery3TB;
	*ECA_2.dc_source_a_tb = FALSE; // Initialize to off
	ECA_2.dc_source_b_tb = &DSCBattery4TB;
	*ECA_2.dc_source_b_tb = FALSE;

	// Descent battery TBs
	DSCBattery1TB.WireTo(&ECA_1);
	DSCBattery2TB.WireTo(&ECA_1);
	DSCBattery3TB.WireTo(&ECA_2);
	DSCBattery4TB.WireTo(&ECA_2);

	// Output CBs (Doesn't work?)
	LMPBatteryFeedTieCB2.WireTo(&ECA_1);

	// Temporarily wire direct to the ECAs.
	// Apparently unpowered (Wired to NULL) busses get 28V for some reason...
	CDRs28VBus.WireTo(&ECA_2);
	LMPs28VBus.WireTo(&ECA_1);

	// RCS Main Shutoff valves
	RCSMainSovASwitch.WireTo(&CDRs28VBus);
	RCSMainSovATB.WireTo(&CDRs28VBus);
	RCSMainSovBTB.WireTo(&LMPs28VBus);
	RCSMainSovBSwitch.WireTo(&LMPs28VBus);
	// RCSMainSovBSwitch.WireTo(&LMPBatteryFeedTieCB2);

	// The IMU (TEMPORARY - HAX)
	imu.WireToBuses(&CDRs28VBus, &LMPs28VBus);
	// FDAI (TEMPORARY - HAX)
	fdaiLeft.WireTo(&CDRs28VBus);

	// Arrange for updates
	Panelsdk.AddElectrical(&ECA_1, false);
	Panelsdk.AddElectrical(&ECA_2, false);
	
	Panelsdk.AddElectrical(&CDRs28VBus, false);
	Panelsdk.AddElectrical(&LMPs28VBus, false);

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
}

void sat5_lmpkd::SystemsTimestep(double simt, double simdt) 

{
	// Joystick read
	if(js_enabled > 0 && oapiGetFocusInterface() == this){
	// CHECK FOR POWER HERE
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
		bool out_of_detent = FALSE;

		// Read data
		HRESULT hr;
		if(rhc_id != -1 && dx8_joystick[rhc_id] != NULL){
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
				out_of_detent = TRUE;
				rhc_pos[0] = dx8_jstate[rhc_id].lX-34028; // Results are 0 - 31507
			}
			if(dx8_jstate[rhc_id].lX < 31508){ // Out of detent LEFT
				out_of_detent = TRUE;
				rhc_pos[0] = dx8_jstate[rhc_id].lX-31508; // Results are 0 - -31508
			}
			if(dx8_jstate[rhc_id].lY > 34028){ // Out of detent UP
				out_of_detent = TRUE;
				rhc_pos[1] = dx8_jstate[rhc_id].lY-34028; // Results are 0 - 31507
			}
			if(dx8_jstate[rhc_id].lY < 31508){ // Out of detent DOWN
				out_of_detent = TRUE;
				rhc_pos[1] = dx8_jstate[rhc_id].lY-31508; // Results are 0 - -31508
			}
			// YAW IS REVERSED
			if(rhc_rot_pos > 34028){ // Out of detent RIGHT
				out_of_detent = TRUE;
				rhc_pos[2] = 34028-rhc_rot_pos; // Results are 0 - 31507
			}
			if(rhc_rot_pos < 31508){ // Out of detent LEFT
				out_of_detent = TRUE;
				rhc_pos[2] = 31508-rhc_rot_pos; // Results are 0 - -31508
			}
		}else{
			// No JS
		}
		if(out_of_detent == TRUE){
			agc.SetInputChannelBit(031,15,1); // ACA OUT OF DETENT
		}else{
			agc.SetInputChannelBit(031,15,0); 
		}
		// sprintf(oapiDebugString(),"RHC: POS %d = %d",rhc_rot_pos,rhc_pos[2]);

	}
	// Each timestep is passed to the SPSDK
	// to perform internal computations on the 
	// systems.
	Panelsdk.Timestep(simt);

	// After that come all other systems simesteps
	fdaiLeft.Timestep(MissionTime, simdt);
}

// PANEL SDK SUPPORT
void sat5_lmpkd::SetValveState(int valve, bool open)

{
	ValveState[valve] = open;

	int valve_state = open ? SP_VALVE_OPEN : SP_VALVE_CLOSE;

	if (pLEMValves[valve])
		*pLEMValves[valve] = valve_state;

	/*
	CheckRCSState();
	*/
}

bool sat5_lmpkd::GetValveState(int valve)

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

void LEM_ECA::Init(sat5_lmpkd *s,e_object *hi_a,e_object *hi_b,e_object *lo_a,e_object *lo_b){
	lem = s;
	input_a = 0;
	input_b = 0;
	dc_source_hi_a = hi_a;
	dc_source_hi_b = hi_b;
	dc_source_lo_a = lo_a;
	dc_source_lo_b = lo_b;
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
	double PowerDrawPerSource = power_load; // Current to draw, per source
	
	// Find active sources
	switch(input_a){
		case 1:
			if(dc_source_hi_a != NULL){
				csrc++;
			}
			break;
		case 2:
			if(dc_source_lo_a != NULL){
				csrc++;
			}
			break;
	}
	switch(input_b){
		case 1:
			if(dc_source_hi_b != NULL){
				csrc++;
			}
			break;
		case 2:
			if(dc_source_lo_b != NULL){
				csrc++;
			}
			break;
	}
	// Compute draw
	if(csrc > 1){
		PowerDrawPerSource /= 2;
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
	
	// Reset for next pass.
	e_object::UpdateFlow(dt);

	double A_Volts = 0;
	double A_Amperes = 0;
	double B_Volts = 0;
	double B_Amperes = 0;

	// Resupply from source
	switch(input_a){
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
	if(csrc > 1){
		Volts = (A_Volts + B_Volts) / 2;
		Amperes = A_Amperes+B_Amperes;
	}else{
		if(input_a != 0){ // Only one (or no) input
			Volts = A_Volts;
			Amperes = A_Amperes;
		}else{
			Volts = B_Volts;
			Amperes = B_Amperes;
		}
	}
	
	//sprintf(oapiDebugString(),"LM_ECA: = Inputs %d %d Voltages %f %f | Load %f Output %f V",input_a,input_b,A_Volts,B_Volts,power_load,Volts);
}

void sat5_lmpkd::CheckRCS()
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
