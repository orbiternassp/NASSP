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

	th_att_rot[2]=CreateThruster (_V(2,2,-2), _V(0,0,1),   MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[4]=CreateThruster (_V(-2,2,-2), _V(0,0,1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[3]=CreateThruster (_V(2,-2,-2), _V(0,0,1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[5]=CreateThruster (_V(-2,-2,-2), _V(0,0,1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[7]=CreateThruster (_V(2,2,2), _V(0,0,-1),   MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[0]=CreateThruster (_V(-2,2,2), _V(0,0,-1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[6]=CreateThruster (_V(2,-2,2), _V(0,0,-1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[1]=CreateThruster (_V(-2,-2,2), _V(0,0,-1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	CreateThrusterGroup (th_att_rot,   4, THGROUP_ATT_YAWLEFT);
	CreateThrusterGroup (th_att_rot+4,   4, THGROUP_ATT_YAWRIGHT);

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
}

void sat5_lmpkd::AddRCS_LMH(double TRANZ)
{

	UINT atthand;
	const double ATTCOOR = 1.78;
	const double ATTCOOR2 = 1.35;
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

//	int MaxThrust=120;
//	double RCSISP=15000;
	double MaxThrust=445.0;
	double RCSISP=2840.0;

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

	th_att_rot[2]=CreateThruster (_V(2,2,-2), _V(0,0,1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[4]=CreateThruster (_V(-2,2,-2), _V(0,0,1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[3]=CreateThruster (_V(2,-2,-2), _V(0,0,1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[5]=CreateThruster (_V(-2,-2,-2), _V(0,0,1),MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[7]=CreateThruster (_V(2,2,2), _V(0,0,-1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[0]=CreateThruster (_V(-2,2,2), _V(0,0,-1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[6]=CreateThruster (_V(2,-2,2), _V(0,0,-1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[1]=CreateThruster (_V(-2,-2,2), _V(0,0,-1),MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	CreateThrusterGroup (th_att_rot,   4, THGROUP_ATT_YAWLEFT);
	CreateThrusterGroup (th_att_rot+4,   4, THGROUP_ATT_YAWRIGHT);

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

	th_att_rot[8]=CreateThruster (_V(2,2,-2), _V(0,-1,0),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[9]=CreateThruster (_V(-2,2,-2), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[12]=CreateThruster (_V(2,2,2), _V(0,-1,0),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[13]=CreateThruster (_V(-2,2,2), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);

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


		//lin Forward

		VECTOR3	m_exhaust_pos2= _V(ATTCOOR-.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8);
		VECTOR3	m_exhaust_ref2 = _V(0,0,-1);
		VECTOR3	m_exhaust_pos3= _V(-ATTCOOR+.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8);
		VECTOR3 m_exhaust_ref3 = _V(0,0,-1);


		//just for vector init
		VECTOR3 m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		VECTOR3 m_exhaust_pos5= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		VECTOR3 m_exhaust_ref4 = _V(0,0.0,1);
		VECTOR3 m_exhaust_ref5 = _V(0,0.0,1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);

		//lin backward
		m_exhaust_pos2= _V(-ATTCOOR+.05,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10);
		m_exhaust_ref2 = _V(-0,0,1);
		m_exhaust_pos3= _V(ATTCOOR-.001,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10);
		m_exhaust_ref3 = _V(-0,0,1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);

		//lin Right
		m_exhaust_pos2= _V(-ATTCOOR-0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25);
		m_exhaust_ref2 = _V(-1,0,0);
		m_exhaust_pos3= _V(-ATTCOOR-0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17);
		m_exhaust_ref3 = _V(-1,0,0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);

		//lin Left
		m_exhaust_pos2= _V(ATTCOOR+0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17);
		m_exhaust_ref2 = _V(1,0,0);
		m_exhaust_pos3= _V(ATTCOOR+0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25);
		m_exhaust_ref3 = _V(1,0,0);


		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);

		//lin up

		m_exhaust_pos2= _V(ATTCOOR-0.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.35);
		m_exhaust_pos3= _V(-ATTCOOR+.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.30);
		m_exhaust_pos4= _V(ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_pos5= _V(-ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref2 = _V(0,1,0);
		m_exhaust_ref3 = _V(0,1,0);
		m_exhaust_ref4 = _V(0,1,0);
		m_exhaust_ref5 = _V(0,1,0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);

		//lin down
		m_exhaust_pos2= _V(ATTCOOR-0.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.35);
		m_exhaust_pos3= _V(-ATTCOOR+.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.3);
		m_exhaust_pos4= _V(ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_pos5= _V(-ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref2 = _V(0.35,-1,-0.35);
		m_exhaust_ref3 = _V(-0.35,-1,-0.35);
		m_exhaust_ref4 = _V(0.35,-1,0.35);
		m_exhaust_ref5 = _V(-0.35,-1,0.35);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);

		//Rot up

		m_exhaust_pos2= _V(ATTCOOR-0.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.35);
		m_exhaust_pos3= _V(-ATTCOOR+.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.30);
		m_exhaust_ref2 = _V(0,1,0);
		m_exhaust_ref3 = _V(0,1,0);
		m_exhaust_pos4= _V(ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_pos5= _V(-ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref4 = _V(0.35,-1,0.35);
		m_exhaust_ref5 = _V(-0.35,-1,0.35);


		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand =AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);

		//Rot down
		m_exhaust_pos4= _V(ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_pos5= _V(-ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref4 = _V(0,1,0);
		m_exhaust_ref5 = _V(0,1,0);

		m_exhaust_pos2= _V(ATTCOOR-0.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.35);
		m_exhaust_pos3= _V(-ATTCOOR+.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.3);
		m_exhaust_ref2 = _V(0.35,-1,-0.35);
		m_exhaust_ref3 = _V(-0.35,-1,-0.35);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);

		//Rot left

		m_exhaust_pos2= _V(ATTCOOR+0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17);
		m_exhaust_ref2 = _V(1,0,0);
		m_exhaust_pos3= _V(ATTCOOR-.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8);
		m_exhaust_ref3 = _V(0,0,-1);
		m_exhaust_pos4= _V(-ATTCOOR-0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25);
		m_exhaust_ref4 = _V(-1,0,0);
		m_exhaust_pos5= _V(-ATTCOOR+.05,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10);
		m_exhaust_ref5 = _V(-0,0,1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);

		//Rot right
		m_exhaust_pos2= _V(-ATTCOOR-0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17);
		m_exhaust_ref2 = _V(-1,0,0);
		m_exhaust_pos3= _V(-ATTCOOR+.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8);
		m_exhaust_ref3 = _V(0,0,-1);
		m_exhaust_pos4= _V(ATTCOOR+0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25);
		m_exhaust_ref4 = _V(1,0,0);
		m_exhaust_pos5= _V(ATTCOOR-.05,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10);
		m_exhaust_ref5 = _V(0,0,1);


		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);

		//Roll left
		m_exhaust_pos2= _V(ATTCOOR-0.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.35);
		m_exhaust_pos4= _V(ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref2 = _V(0,1,0);
		m_exhaust_ref4 = _V(0,1,0);

		m_exhaust_pos3= _V(-ATTCOOR+.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.3);
		m_exhaust_pos5= _V(-ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref3 = _V(-0.35,-1,-0.35);
		m_exhaust_ref5 = _V(-0.35,-1,0.35);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);

		//Roll right

		m_exhaust_pos3= _V(-ATTCOOR+.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.30);
		m_exhaust_pos5= _V(-ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref3 = _V(0,1,0);
		m_exhaust_ref5 = _V(0,1,0);
		m_exhaust_pos2= _V(ATTCOOR-0.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.35);
		m_exhaust_pos4= _V(ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref2 = _V(0.35,-1,-0.35);
		m_exhaust_ref4 = _V(0.35,-1,0.35);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
}

void sat5_lmpkd::AddRCS_LMH2(double TRANZ)
{
	UINT atthand;
	const double ATTCOOR = 1.78;
	const double ATTCOOR2 = -0.50;
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

//	int MaxThrust=120;
//	double RCSISP=15000;
	double MaxThrust=445.0;
	double RCSISP=2840.0;

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

	th_att_rot[2]=CreateThruster (_V(2,2,-2), _V(0,0,1),   MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[4]=CreateThruster (_V(-2,2,-2), _V(0,0,1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[3]=CreateThruster (_V(2,-2,-2), _V(0,0,1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[5]=CreateThruster (_V(-2,-2,-2), _V(0,0,1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[7]=CreateThruster (_V(2,2,2), _V(0,0,-1),   MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[0]=CreateThruster (_V(-2,2,2), _V(0,0,-1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[6]=CreateThruster (_V(2,-2,2), _V(0,0,-1),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[1]=CreateThruster (_V(-2,-2,2), _V(0,0,-1), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	CreateThrusterGroup (th_att_rot,   4, THGROUP_ATT_YAWLEFT);
	CreateThrusterGroup (th_att_rot+4,   4, THGROUP_ATT_YAWRIGHT);

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

	th_att_rot[8]=CreateThruster (_V(2,2,-2), _V(0,-1,0),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[9]=CreateThruster (_V(-2,2,-2), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[12]=CreateThruster (_V(2,2,2), _V(0,-1,0),  MaxThrust, ph_rcslm1, RCSISP, RCSISP);
	th_att_rot[13]=CreateThruster (_V(-2,2,2), _V(0,-1,0), MaxThrust, ph_rcslm1, RCSISP, RCSISP);

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

		VECTOR3	m_exhaust_pos2= _V(ATTCOOR-.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8);
		VECTOR3	m_exhaust_ref2 = _V(0,0,-1);
		VECTOR3	m_exhaust_pos3= _V(-ATTCOOR+.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8);
		VECTOR3 m_exhaust_ref3 = _V(0,0,-1);


		//just for vector init
		VECTOR3 m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		VECTOR3 m_exhaust_pos5= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		VECTOR3 m_exhaust_ref4 = _V(0,0.0,1);
		VECTOR3 m_exhaust_ref5 = _V(0,0.0,1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);

		//lin backward
		m_exhaust_pos2= _V(-ATTCOOR+.05,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10);
		m_exhaust_ref2 = _V(-0,0,1);
		m_exhaust_pos3= _V(ATTCOOR-.001,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10);
		m_exhaust_ref3 = _V(-0,0,1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);

		//lin Right
		m_exhaust_pos2= _V(-ATTCOOR-0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25);
		m_exhaust_ref2 = _V(-1,0,0);
		m_exhaust_pos3= _V(-ATTCOOR-0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17);
		m_exhaust_ref3 = _V(-1,0,0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);

		//lin Left
		m_exhaust_pos2= _V(ATTCOOR+0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17);
		m_exhaust_ref2 = _V(1,0,0);
		m_exhaust_pos3= _V(ATTCOOR+0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25);
		m_exhaust_ref3 = _V(1,0,0);


		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);

		//lin up

		m_exhaust_pos2= _V(ATTCOOR-0.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.35);
		m_exhaust_pos3= _V(-ATTCOOR+.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.30);
		m_exhaust_pos4= _V(ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_pos5= _V(-ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref2 = _V(0,1,0);
		m_exhaust_ref3 = _V(0,1,0);
		m_exhaust_ref4 = _V(0,1,0);
		m_exhaust_ref5 = _V(0,1,0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);

		//lin down
		m_exhaust_pos2= _V(ATTCOOR-0.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.35);
		m_exhaust_pos3= _V(-ATTCOOR+.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.3);
		m_exhaust_pos4= _V(ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_pos5= _V(-ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref2 = _V(0.0,-1,-0.0);
		m_exhaust_ref3 = _V(-0.0,-1,-0.00);
		m_exhaust_ref4 = _V(0.0,-1,0.0);
		m_exhaust_ref5 = _V(-0.0,-1,0.0);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);

		//Rot up

		m_exhaust_pos2= _V(ATTCOOR-0.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.35);
		m_exhaust_pos3= _V(-ATTCOOR+.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.30);
		m_exhaust_ref2 = _V(0,1,0);
		m_exhaust_ref3 = _V(0,1,0);
		m_exhaust_pos4= _V(ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_pos5= _V(-ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref4 = _V(0.35,-1,0.35);
		m_exhaust_ref5 = _V(-0.35,-1,0.35);


		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand =AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);

		//Rot down
		m_exhaust_pos4= _V(ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_pos5= _V(-ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref4 = _V(0,1,0);
		m_exhaust_ref5 = _V(0,1,0);

		m_exhaust_pos2= _V(ATTCOOR-0.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.35);
		m_exhaust_pos3= _V(-ATTCOOR+.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.3);
		m_exhaust_ref2 = _V(0.35,-1,-0.35);
		m_exhaust_ref3 = _V(-0.35,-1,-0.35);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);

		//Rot left

		m_exhaust_pos2= _V(ATTCOOR+0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17);
		m_exhaust_ref2 = _V(1,0,0);
		m_exhaust_pos3= _V(ATTCOOR-.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8);
		m_exhaust_ref3 = _V(0,0,-1);
		m_exhaust_pos4= _V(-ATTCOOR-0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25);
		m_exhaust_ref4 = _V(-1,0,0);
		m_exhaust_pos5= _V(-ATTCOOR+.05,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10);
		m_exhaust_ref5 = _V(-0,0,1);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);

		//Rot right
		m_exhaust_pos2= _V(-ATTCOOR-0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17);
		m_exhaust_ref2 = _V(-1,0,0);
		m_exhaust_pos3= _V(-ATTCOOR+.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8);
		m_exhaust_ref3 = _V(0,0,-1);
		m_exhaust_pos4= _V(ATTCOOR+0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25);
		m_exhaust_ref4 = _V(1,0,0);
		m_exhaust_pos5= _V(ATTCOOR-.05,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10);
		m_exhaust_ref5 = _V(0,0,1);


		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);

		//Roll left
		m_exhaust_pos2= _V(ATTCOOR-0.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.35);
		m_exhaust_pos4= _V(ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref2 = _V(0,1,0);
		m_exhaust_ref4 = _V(0,1,0);

		m_exhaust_pos3= _V(-ATTCOOR+.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.3);
		m_exhaust_pos5= _V(-ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref3 = _V(-0.35,-1,-0.35);
		m_exhaust_ref5 = _V(-0.35,-1,0.35);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);

		//Roll right

		m_exhaust_pos3= _V(-ATTCOOR+.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.30);
		m_exhaust_pos5= _V(-ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref3 = _V(0,1,0);
		m_exhaust_ref5 = _V(0,1,0);
		m_exhaust_pos2= _V(ATTCOOR-0.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.35);
		m_exhaust_pos4= _V(ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3);
		m_exhaust_ref2 = _V(0.35,-1,-0.35);
		m_exhaust_ref4 = _V(0.35,-1,0.35);

		atthand = AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
}

void sat5_lmpkd::SetRCS(PROPELLANT_HANDLE ph_prop)
{
		if(ATT2switch && QUAD1switch && QUAD2switch && QUAD3switch  && QUAD4switch  && QUAD5switch  && QUAD6switch  && QUAD7switch  && QUAD8switch && MSOV1switch && MSOV2switch && ED1switch && ED4switch && RCSS1switch && RCSS2switch && RCSS3switch && RCSS4switch){
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
		if(ATT3switch && QUAD1switch && QUAD2switch && QUAD3switch  && QUAD4switch  && QUAD5switch  && QUAD6switch  && QUAD7switch  && QUAD8switch && MSOV1switch && MSOV2switch && ED1switch && ED4switch && RCSS1switch && RCSS2switch && RCSS3switch && RCSS4switch){
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
		if(ATT1switch && QUAD1switch && QUAD2switch && QUAD3switch  && QUAD4switch  && QUAD5switch  && QUAD6switch  && QUAD7switch  && QUAD8switch && MSOV1switch && MSOV2switch && ED1switch && ED4switch && RCSS1switch && RCSS2switch && RCSS3switch && RCSS4switch){
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
}

bool sat5_lmpkd::CabinFansActive()

{
	return CABFswitch;
}

bool sat5_lmpkd::AscentEngineArmed()

{
	return (EngineArmSwitch.IsUp); //&& !ASCHE1switch && !ASCHE2switch && ED1switch && ED6switch && ED7switch && ED8switch;
}

bool sat5_lmpkd::AscentRCSArmed()

{
	return AFEED1switch || AFEED2switch || AFEED3switch || AFEED4switch;
}
