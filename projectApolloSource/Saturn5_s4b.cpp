/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn V Module Parked/Docked mode

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
  **************************************************************************/

#include "orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"


//Begin code


void ResetThrusters(VESSEL *vessel){
	vessel->SetAttitudeRotLevel(0, 0);
	vessel->SetAttitudeRotLevel(1, 0);
	vessel->SetAttitudeRotLevel(2, 0);
	vessel->SetAttitudeLinLevel(0, 0);
	vessel->SetAttitudeLinLevel(1, 0);
	vessel->SetAttitudeLinLevel(2, 0);
	vessel->ActivateNavmode(NAVMODE_KILLROT);

}
void AddRCS_LM(VESSEL *vessel,double TRANZ)
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

		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);

		//lin backward

		m_exhaust_pos2= _V(ATTCOOR2+0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0,0.0,1);
		m_exhaust_ref5 = _V(0,0.0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);

		//lin Left

		m_exhaust_pos2= _V(-ATTCOOR2-0.27,-ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2-0.27,ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,1,0);
		m_exhaust_ref3 = _V(-1,-1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);

		//lin right

		m_exhaust_pos2= _V(ATTCOOR2+0.27,-ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(ATTCOOR2+0.27,ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(1,1,0);
		m_exhaust_ref3 = _V(1,-1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);

		//lin down

		m_exhaust_pos2= _V(ATTCOOR2,ATTCOOR2+0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2,ATTCOOR2+0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,1,0);
		m_exhaust_ref3 = _V(1,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		//lin up
		m_exhaust_pos2= _V(ATTCOOR2,-ATTCOOR2-0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2,-ATTCOOR2-0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,-1,0);
		m_exhaust_ref3 = _V(1,-1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);

		//Rot up

		m_exhaust_pos2= _V(ATTCOOR2+0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0.35,-0.35,-1);
		m_exhaust_ref5 = _V(-0.35,-0.35,-1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);

		//Rot down
		m_exhaust_pos2= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0.35,0.35,-1);
		m_exhaust_ref5 = _V(-0.35,0.35,-1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);


		//Rot left

		m_exhaust_pos2= _V(-ATTCOOR2-0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(ATTCOOR2+0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0.35,-0.35,-1);
		m_exhaust_ref5 = _V(0.35,0.35,-1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);

		//Rot right

		m_exhaust_pos2= _V(ATTCOOR2+0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,ATTCOOR2+0.1,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(-0.35,-0.35,-1);
		m_exhaust_ref5 = _V(-0.35,0.35,-1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);


		//Roll left

		m_exhaust_pos2= _V(ATTCOOR2+0.27,ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2-0.27,-ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_pos4= _V(ATTCOOR2,-ATTCOOR2-0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(1,-1,0);
		m_exhaust_ref3 = _V(-1,1,0);
		m_exhaust_ref4 = _V(-1,-1,0);
		m_exhaust_pos5= _V(-ATTCOOR2,ATTCOOR2+0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_ref5 = _V(1,1,0);

		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		//Roll right
		m_exhaust_pos2= _V(-ATTCOOR2-0.27,ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(ATTCOOR2+0.27,-ATTCOOR2,TRANZ+RCSOFFSETM2);
		m_exhaust_pos4= _V(ATTCOOR2,ATTCOOR2+0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_pos5= _V(-ATTCOOR2,-ATTCOOR2-0.27,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,-1,0);
		m_exhaust_ref3 = _V(1,1,0);
		m_exhaust_ref4 = _V(-1,1,0);
		m_exhaust_ref5 = _V(1,-1,0);

		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);



}
void AddRCS_LM2(VESSEL *vessel,double TRANZ)
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

		//lin Forward

		VECTOR3 m_exhaust_pos2= _V(ATTCOOR2+0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_pos3= _V(-ATTCOOR2-0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_pos4= _V(ATTCOOR2+0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_pos5= _V(-ATTCOOR2-0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_ref2 = _V(0.35,-0.35,-1);
		VECTOR3 m_exhaust_ref3 = _V(-0.35,-0.35,-1);
		VECTOR3 m_exhaust_ref4 = _V(0.35,0.35,-1);
		VECTOR3 m_exhaust_ref5 = _V(-0.35,0.35,-1);

		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);

		//lin backward

		m_exhaust_pos2= _V(ATTCOOR2+0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0,0.0,1);
		m_exhaust_ref5 = _V(0,0.0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);

		//lin Left

		m_exhaust_pos2= _V(-ATTCOOR2-0.27,-ATTCOORY2+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2-0.27,ATTCOOR2-.28+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,1,0);
		m_exhaust_ref3 = _V(-1,-1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);

		//lin right

		m_exhaust_pos2= _V(ATTCOOR2+0.27,-ATTCOORY2+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(ATTCOOR2+0.27,ATTCOOR2-.28+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(1,1,0);
		m_exhaust_ref3 = _V(1,-1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);

		//lin down

		m_exhaust_pos2= _V(ATTCOOR2,ATTCOOR2-.28+0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2,ATTCOOR2-.28+0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,1,0);
		m_exhaust_ref3 = _V(1,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		//lin up
		m_exhaust_pos2= _V(ATTCOOR2,-ATTCOORY2-0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2,-ATTCOORY2-0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,-1,0);
		m_exhaust_ref3 = _V(1,-1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);

		//Rot up

		m_exhaust_pos2= _V(ATTCOOR2+0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0.35,-0.35,-1);
		m_exhaust_ref5 = _V(-0.35,-0.35,-1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);

		//Rot down
		m_exhaust_pos2= _V(ATTCOOR2+0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0.35,0.35,-1);
		m_exhaust_ref5 = _V(-0.35,0.35,-1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);


		//Rot left

		m_exhaust_pos2= _V(-ATTCOOR2-0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(-ATTCOOR2-0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(ATTCOOR2+0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(0.35,-0.35,-1);
		m_exhaust_ref5 = _V(0.35,0.35,-1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);

		//Rot right

		m_exhaust_pos2= _V(ATTCOOR2+0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos3= _V(ATTCOOR2+0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET);
		m_exhaust_pos4= _V(-ATTCOOR2-0.1,-ATTCOORY2-0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_pos5= _V(-ATTCOOR2-0.1,ATTCOOR2-.28+0.1+LM2VOFS,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref2 = _V(0,0.0,1);
		m_exhaust_ref3 = _V(0,0.0,1);
		m_exhaust_ref4 = _V(-0.35,-0.35,-1);
		m_exhaust_ref5 = _V(-0.35,0.35,-1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);


		//Roll left

		m_exhaust_pos2= _V(ATTCOOR2+0.27,ATTCOOR2-.28+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2-0.27,-ATTCOORY2+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos4= _V(ATTCOOR2,-ATTCOORY2-0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(1,-1,0);
		m_exhaust_ref3 = _V(-1,1,0);
		m_exhaust_ref4 = _V(-1,-1,0);
		m_exhaust_pos5= _V(-ATTCOOR2,ATTCOOR2-.28+0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref5 = _V(1,1,0);

		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		//Roll right
		m_exhaust_pos2= _V(-ATTCOOR2-0.27,ATTCOOR2-.28+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(ATTCOOR2+0.27,-ATTCOORY2+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos4= _V(ATTCOOR2,ATTCOOR2-.28+0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_pos5= _V(-ATTCOOR2,-ATTCOORY2-0.27+LM2VOFS,TRANZ+RCSOFFSETM2);
		m_exhaust_ref2 = _V(-1,-1,0);
		m_exhaust_ref3 = _V(1,1,0);
		m_exhaust_ref4 = _V(-1,1,0);
		m_exhaust_ref5 = _V(1,-1,0);

		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);



}
void AddRCS_LMH(VESSEL *vessel,double TRANZ)
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

		VECTOR3 m_exhaust_pos2= _V(ATTCOOR2,ATTCOOR2-0.33,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_pos3= _V(-ATTCOOR2,ATTCOOR2-0.33,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_ref2 = _V(-1,0,-1);
		VECTOR3 m_exhaust_ref3 = _V(1,0,-1);
		//just for vector init
		VECTOR3 m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		VECTOR3 m_exhaust_pos5= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		VECTOR3 m_exhaust_ref4 = _V(0,0.0,1);
		VECTOR3 m_exhaust_ref5 = _V(0,0.0,1);

		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);

		//lin backward

		m_exhaust_pos2= _V(ATTCOOR2,ATTCOOR2-0.33,TRANZ+RCSOFFSET+2.9);
		m_exhaust_pos3= _V(-ATTCOOR2,ATTCOOR2-0.33,TRANZ+RCSOFFSET+2.9);
		m_exhaust_ref2 = _V(-1,0,1);
		m_exhaust_ref3 = _V(1,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);

		//lin Right

		m_exhaust_pos2= _V(-ATTCOOR2-0.33,ATTCOOR2-0.33,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2-0.33,ATTCOOR2-0.33,TRANZ+RCSOFFSETM2+2.9);
		m_exhaust_ref2 = _V(-1,0,0.75);
		m_exhaust_ref3 = _V(-1,0,-0.75);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);

		//lin Left

		m_exhaust_pos2= _V(ATTCOOR2+0.33,ATTCOOR2-0.33,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(ATTCOOR2+0.33,ATTCOOR2-0.33,TRANZ+RCSOFFSETM2+2.9);
		m_exhaust_ref2 = _V(1,0,0.75);
		m_exhaust_ref3 = _V(1,0,-0.75);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);

		//lin up

		m_exhaust_pos2= _V(ATTCOOR2+.1,ATTCOOR2-0.1,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos3= _V(-ATTCOOR2-.1,ATTCOOR2-0.1,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos4= _V(ATTCOOR2+.1,ATTCOOR2-0.1,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_pos5= _V(-ATTCOOR2-.1,ATTCOOR2-0.1,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref2 = _V(0,1,0);
		m_exhaust_ref3 = _V(0,1,0);
		m_exhaust_ref4 = _V(0,1,0);
		m_exhaust_ref5 = _V(0,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		//lin down
		m_exhaust_pos2= _V(ATTCOOR2+.1,ATTCOOR2-.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos3= _V(-ATTCOOR2-.1,ATTCOOR2-.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos4= _V(ATTCOOR2+.1,ATTCOOR2-.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_pos5= _V(-ATTCOOR2-.1,ATTCOOR2-.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref2 = _V(0.35,-1,-0.35);
		m_exhaust_ref3 = _V(-0.35,-1,-0.35);
		m_exhaust_ref4 = _V(0.35,-1,0.35);
		m_exhaust_ref5 = _V(-0.35,-1,0.35);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		//Rot up

		m_exhaust_pos2= _V(ATTCOOR2+.1,ATTCOOR2-0.1,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos3= _V(-ATTCOOR2-.1,ATTCOOR2-0.1,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_ref2 = _V(0,1,0);
		m_exhaust_ref3 = _V(0,1,0);
		m_exhaust_pos4= _V(ATTCOOR2+.1,ATTCOOR2-.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_pos5= _V(-ATTCOOR2-.1,ATTCOOR2-.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref4 = _V(0.35,-1,0.35);
		m_exhaust_ref5 = _V(-0.35,-1,0.35);


		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);

		//Rot down
		m_exhaust_pos4= _V(ATTCOOR2+.1,ATTCOOR2-0.1,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_pos5= _V(-ATTCOOR2-.1,ATTCOOR2-0.1,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref4 = _V(0,1,0);
		m_exhaust_ref5 = _V(0,1,0);


		m_exhaust_pos2= _V(ATTCOOR2+.1,ATTCOOR2-.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos3= _V(-ATTCOOR2-.1,ATTCOOR2-.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_ref2 = _V(0.35,-1,-0.35);
		m_exhaust_ref3 = _V(-0.35,-1,-0.35);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);


		//Rot left

		m_exhaust_pos2= _V(ATTCOOR2+0.33,ATTCOOR2-0.33,TRANZ+RCSOFFSETM2+2.9);
		m_exhaust_ref2 = _V(1,0,-1);
		m_exhaust_pos3= _V(ATTCOOR2,ATTCOOR2-0.33,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref3 = _V(-1,0,-1);
		m_exhaust_pos4= _V(-ATTCOOR2-0.33,ATTCOOR2-0.33,TRANZ+RCSOFFSETM2);
		m_exhaust_ref4 = _V(-1,0,1);
		m_exhaust_pos5= _V(-ATTCOOR2,ATTCOOR2-0.33,TRANZ+RCSOFFSET+2.9);
		m_exhaust_ref5 = _V(1,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);

		//Rot right

		m_exhaust_pos2= _V(-ATTCOOR2-0.33,ATTCOOR2-0.33,TRANZ+RCSOFFSETM2+2.9);
		m_exhaust_ref2 = _V(-1,0,-1);
		m_exhaust_pos3= _V(-ATTCOOR2,ATTCOOR2-0.33,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref3 = _V(1,0,-1);
		m_exhaust_pos4= _V(ATTCOOR2+0.33,ATTCOOR2-0.33,TRANZ+RCSOFFSETM2);
		m_exhaust_ref4 = _V(1,0,1);
		m_exhaust_pos5= _V(ATTCOOR2,ATTCOOR2-0.33,TRANZ+RCSOFFSET+2.9);
		m_exhaust_ref5 = _V(-1,0,1);

		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);


		//Roll left

		m_exhaust_pos2= _V(ATTCOOR2+.1,ATTCOOR2-0.1,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos4= _V(ATTCOOR2+.1,ATTCOOR2-0.1,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref2 = _V(0,1,0);
		m_exhaust_ref4 = _V(0,1,0);




		m_exhaust_pos3= _V(-ATTCOOR2-.1,ATTCOOR2-.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos5= _V(-ATTCOOR2-.1,ATTCOOR2-.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref3 = _V(-0.35,-1,-0.35);
		m_exhaust_ref5 = _V(-0.35,-1,0.35);

		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		//Roll right

		m_exhaust_pos3= _V(-ATTCOOR2-.1,ATTCOOR2-0.1,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos5= _V(-ATTCOOR2-.1,ATTCOOR2-0.1,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref3 = _V(0,1,0);
		m_exhaust_ref5 = _V(0,1,0);
		m_exhaust_pos2= _V(ATTCOOR2+.1,ATTCOOR2-.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos4= _V(ATTCOOR2+.1,ATTCOOR2-.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref2 = _V(0.35,-1,-0.35);
		m_exhaust_ref4 = _V(0.35,-1,0.35);


		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);



}
void AddRCS_LMH2(VESSEL *vessel,double TRANZ)
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

		VECTOR3 m_exhaust_pos2= _V(ATTCOOR2,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_pos3= _V(-ATTCOOR2,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSET-0.6);
		VECTOR3 m_exhaust_ref2 = _V(-1,0,-1);
		VECTOR3 m_exhaust_ref3 = _V(1,0,-1);
		//just for vector init
		VECTOR3 m_exhaust_pos4= _V(ATTCOOR2+0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		VECTOR3 m_exhaust_pos5= _V(-ATTCOOR2-0.1,-ATTCOOR2-0.1,TRANZ+RCSOFFSET);
		VECTOR3 m_exhaust_ref4 = _V(0,0.0,1);
		VECTOR3 m_exhaust_ref5 = _V(0,0.0,1);

		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,0);

		//lin backward

		m_exhaust_pos2= _V(ATTCOOR2,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSET+2.9);
		m_exhaust_pos3= _V(-ATTCOOR2,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSET+2.9);
		m_exhaust_ref2 = _V(-1,0,1);
		m_exhaust_ref3 = _V(1,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,2,1);

		//lin Right

		m_exhaust_pos2= _V(-ATTCOOR2-0.33,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(-ATTCOOR2-0.33,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSETM2+2.9);
		m_exhaust_ref2 = _V(-1,0,0.75);
		m_exhaust_ref3 = _V(-1,0,-0.75);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,0);

		//lin Left

		m_exhaust_pos2= _V(ATTCOOR2+0.33,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSETM2);
		m_exhaust_pos3= _V(ATTCOOR2+0.33,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSETM2+2.9);
		m_exhaust_ref2 = _V(1,0,0.75);
		m_exhaust_ref3 = _V(1,0,-0.75);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,0,1);

		//lin up

		m_exhaust_pos2= _V(ATTCOOR2+.1,ATTCOOR2-0.1-1.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos3= _V(-ATTCOOR2-.1,ATTCOOR2-0.1-1.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos4= _V(ATTCOOR2+.1,ATTCOOR2-0.1-1.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_pos5= _V(-ATTCOOR2-.1,ATTCOOR2-0.1-1.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref2 = _V(0,1,0);
		m_exhaust_ref3 = _V(0,1,0);
		m_exhaust_ref4 = _V(0,1,0);
		m_exhaust_ref5 = _V(0,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,1);
		//lin down
		m_exhaust_pos2= _V(ATTCOOR2+.1,ATTCOOR2-.6-1.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos3= _V(-ATTCOOR2-.1,ATTCOOR2-.6-1.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos4= _V(ATTCOOR2+.1,ATTCOOR2-.6-1.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_pos5= _V(-ATTCOOR2-.1,ATTCOOR2-.6-1.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref2 = _V(0.0,-1,-0.0);
		m_exhaust_ref3 = _V(-0.,-1,-0.0);
		m_exhaust_ref4 = _V(0.0,-1,0.0);
		m_exhaust_ref5 = _V(-0.0,-1,0.0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_LIN,1,0);
		//Rot up

		m_exhaust_pos2= _V(ATTCOOR2+.1,ATTCOOR2-0.1-1.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos3= _V(-ATTCOOR2-.1,ATTCOOR2-0.1-1.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_ref2 = _V(0,1,0);
		m_exhaust_ref3 = _V(0,1,0);
		m_exhaust_pos4= _V(ATTCOOR2+.1,ATTCOOR2-.6-1.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_pos5= _V(-ATTCOOR2-.1,ATTCOOR2-.6-1.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref4 = _V(0.0,-1,0.0);
		m_exhaust_ref5 = _V(-0.0,-1,0.0);


		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,0);

		//Rot down
		m_exhaust_pos4= _V(ATTCOOR2+.1,ATTCOOR2-0.1-1.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_pos5= _V(-ATTCOOR2-.1,ATTCOOR2-0.1-1.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref4 = _V(0,1,0);
		m_exhaust_ref5 = _V(0,1,0);


		m_exhaust_pos2= _V(ATTCOOR2+.1,ATTCOOR2-.6-1.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos3= _V(-ATTCOOR2-.1,ATTCOOR2-.6-1.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_ref2 = _V(0.0,-1,-0.0);
		m_exhaust_ref3 = _V(-0.0,-1,-0.0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,0,1);


		//Rot left

		m_exhaust_pos2= _V(ATTCOOR2+0.33,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSETM2+2.9);
		m_exhaust_ref2 = _V(1,0,-1);
		m_exhaust_pos3= _V(ATTCOOR2,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref3 = _V(-1,0,-1);
		m_exhaust_pos4= _V(-ATTCOOR2-0.33,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSETM2);
		m_exhaust_ref4 = _V(-1,0,1);
		m_exhaust_pos5= _V(-ATTCOOR2,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSET+2.9);
		m_exhaust_ref5 = _V(1,0,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,0);

		//Rot right

		m_exhaust_pos2= _V(-ATTCOOR2-0.33,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSETM2+2.9);
		m_exhaust_ref2 = _V(-1,0,-1);
		m_exhaust_pos3= _V(-ATTCOOR2,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSET-0.6);
		m_exhaust_ref3 = _V(1,0,-1);
		m_exhaust_pos4= _V(ATTCOOR2+0.33,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSETM2);
		m_exhaust_ref4 = _V(1,0,1);
		m_exhaust_pos5= _V(ATTCOOR2,ATTCOOR2-0.33-1.6,TRANZ+RCSOFFSET+2.9);
		m_exhaust_ref5 = _V(-1,0,1);

		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,1,1);


		//Roll left

		m_exhaust_pos2= _V(ATTCOOR2+.1,ATTCOOR2-0.1-1.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos4= _V(ATTCOOR2+.1,ATTCOOR2-0.1-1.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref2 = _V(0,1,0);
		m_exhaust_ref4 = _V(0,1,0);




		m_exhaust_pos3= _V(-ATTCOOR2-.1,ATTCOOR2-.6-1.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos5= _V(-ATTCOOR2-.1,ATTCOOR2-.6-1.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref3 = _V(-0.0,-1,-0.0);
		m_exhaust_ref5 = _V(-0.0,-1,0.0);

		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,0);
		//Roll right

		m_exhaust_pos3= _V(-ATTCOOR2-.1,ATTCOOR2-0.1-1.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos5= _V(-ATTCOOR2-.1,ATTCOOR2-0.1-1.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref3 = _V(0,1,0);
		m_exhaust_ref5 = _V(0,1,0);
		m_exhaust_pos2= _V(ATTCOOR2+.1,ATTCOOR2-.6-1.6,TRANZ+RCSOFFSETM2-0.22);
		m_exhaust_pos4= _V(ATTCOOR2+.1,ATTCOOR2-.6-1.6,TRANZ+RCSOFFSETM2-0.22+3.3);
		m_exhaust_ref2 = _V(0.0,-1,-0.0);
		m_exhaust_ref4 = _V(0.0,-1,0.0);


		atthand = vessel->AddAttExhaustRef(m_exhaust_pos2,m_exhaust_ref2,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos3,m_exhaust_ref3,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos4,m_exhaust_ref4,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);
		atthand = vessel->AddAttExhaustRef(m_exhaust_pos5,m_exhaust_ref5,ATTWIDTH,ATTHEIGHT);
		vessel->AddAttExhaustMode(atthand,ATTMODE_ROT,2,1);



}
void ToggleEVA(VESSEL *vessel){

	ToggleEva=false;

	if (EVA_IP){
		EVA_IP =false;
		vessel->ClearMeshes();
		vessel->ClearExhaustRefs();
		vessel->ClearAttExhaustRefs();
		VECTOR3 mesh_dir=_V(-0.08,0,0);
		vessel->AddMesh (hLMVessel, &mesh_dir);


	}else{
		EVA_IP =true;


		VESSELSTATUS vs1;
		vessel->GetStatus(vs1);
		VECTOR3 ofs1 = _V(0,0,0);
		VECTOR3 vel1 = _V(0,0,0);
		VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
		vessel->Local2Rel (ofs1, vs1.rpos);
		vessel->GlobalRot (vel1, rofs1);
		vs1.rvel.x = rvel1.x+rofs1.x;
		vs1.rvel.y = rvel1.y+rofs1.y;
		vs1.rvel.z = rvel1.z+rofs1.z;
		//vs1.vdata[0].z = vs1.vdata[0].z +PI;
		char VName[256]="";
		strcpy (VName, vessel->GetName()); strcat (VName, "-LEVA");
		hLEVA = oapiCreateVessel(VName,"Saturn5_LEVA",vs1);
		vessel->ClearMeshes();
		vessel->ClearExhaustRefs();
		vessel->ClearAttExhaustRefs();
		VECTOR3 mesh_dir=_V(-0.08,-0.15,-4.3);
		vessel->AddMesh (hLMVessel, &mesh_dir);
		oapiSetFocusObject(hLEVA);
		PlayVesselWave(SLEVA,NOLOOP,255);
	}
}
void SetupEVA(VESSEL *vessel){



	if (EVA_IP){

		EVA_IP =true;
		vessel->ClearMeshes();
		VECTOR3 mesh_dir=_V(-0.08,-0.15,-4.3);
		vessel->AddMesh (hLMVessel, &mesh_dir);

		}


}


void SetView(VESSEL *vessel){


	if (cdrview)	{
		vessel->SetCameraOffset (_V(-1.02,1.05,-1.67));
		cdrview=false;
	}
	if (lmpview){
		vessel->SetCameraOffset (_V(1.02,1.05,-1.67));
		lmpview=false;
	}

}

void SetLMStage(VESSEL *vessel)
{

		vessel->SetSize (15.5);
		vessel->SetEmptyMass (65600);
		vessel->SetMaxFuelMass (1140000);
		vessel->SetFuelMass(100);
		vessel->SetISP (5159.5);
		vessel->SetMaxThrust (ENGINE_MAIN, 1201725);
		vessel->SetMaxThrust (ENGINE_ATTITUDE, 4700);
		vessel->SetEngineLevel(ENGINE_MAIN, 0.0);
		vessel->ClearMeshes();
		VECTOR3 mesh_dir=_V(0,0,0);
		vessel->AddMesh (hsat5stg3, &mesh_dir);
		if( dockstate <=2){
		mesh_dir=_V(-0.0,0,9.8);
		vessel->AddMesh (hLMPKD, &mesh_dir);
		}



}

// ==============================================================
// API interface
// ==============================================================



DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{

	return new VESSEL (hvessel, flightmodel);
}


DLLCLBK int ovcConsumeKey (VESSEL *vessel, const char *keystate)
{
	if (KEYMOD_SHIFT (keystate))
	{
		return 0;
	}
	else if (KEYMOD_CONTROL (keystate))
	{

	}
	else
	{
		if (KEYDOWN (keystate, OAPI_KEY_J)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_J, 1.0)){
			}
		}
		if (KEYDOWN (keystate, OAPI_KEY_K)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_K, 1.0)){

			return 1;
			}
		}

	}
	return 0;
}

DLLCLBK void ovcTimestep (VESSEL *vessel, double simt)
{



	char VName[256]="";
	char VName1[256]="";
	char VName2[256]="";
	char VName3[256]="";
	char VName4[256]="";
	char VName5[256]="";
	char VName6[256]="";
	char VName7[256]="";
	char VName8[256]="";
	char VName9[256]="";
	char VName10[256]="";
	char VName11[256]="";
	char VName12[256]="";

	strcpy (VName10, vessel->GetName()); strcat (VName10, "-LEVA");
	hLEVA=oapiGetVesselByName(VName10);

		SetView(vessel);


	if (stage == 0)
	{

	}else if (stage == 1||stage == 5)
	{
		if (EVA_IP){
			if(!hLEVA){
			ToggleEVA(vessel);
			}
		}
		if (ToggleEva){
		ToggleEVA(vessel);
		}

		if (bToggleHatch){
				VESSELSTATUS vs;
				vessel->GetStatus(vs);
				if (vs.status == 1){
				PlayVesselWave(Scontact,NOLOOP,255);
				SetLmVesselHoverStage2(vessel);
				}
		bToggleHatch=false;
		}

		if (vessel->GetFuelMass()==408){
			PlayVesselWave(S5P100,NOLOOP,255);
		}
		if (vessel->GetAltitude() < 3.9 && !ContactOK && vessel->GetAltitude() > 3.0){
			PlayVesselWave(Scontact,NOLOOP,255);
			vessel->SetEngineLevel(ENGINE_HOVER,0);
			ContactOK = true;

		}

		if (vessel->GetFuelMass()<=50 && vessel->GetAltitude() > 10){
			SeparateStage(vessel,stage);
			vessel->ActivateNavmode(NAVMODE_HLEVEL);
			stage = 2;
			PlayVesselWave(SAbort,NOLOOP,255);
		}
		if (bManualSeparate && !startimer){
			VESSELSTATUS vs;
			vessel->GetStatus(vs);

			if (vs.status == 1){
			countdown=simt;
			PlayVesselWave(Sctdw,NOLOOP,255);
			startimer=true;
			}else{
			SeparateStage(vessel,stage);
			stage = 2;
			}

		}
			if ((simt-(10+countdown))>=0 && startimer ){
				SeparateStage(vessel,stage);
				stage = 2;
				startimer=false;
			}

	}else if (stage == 2)


	{
	}else if (stage == 3)
	{
	}else if (stage == 4)
	{




	}
	if (velDISP){
	VECTOR3 RVEL = _V(0.0,0.0,0.0);
	vessel->GetRelativeVel(vessel->GetGravityRef(),RVEL);
	double actualVEL;
	actualVEL = (sqrt(RVEL.x *RVEL.x + RVEL.y * RVEL.y + RVEL.z * RVEL.z)/1000*3600);
	sprintf(oapiDebugString(), "Velocity KM/H %d", int(actualVEL));
	}


//sprintf(oapiDebugString(), "nom %s", VNameCM );
}
DLLCLBK void ovcLoadState (VESSEL *vessel, FILEHANDLE scn, VESSELSTATUS *vs)

{
    char *line;

	while (oapiReadScenario_nextline (scn, line)) {

        if (!strnicmp (line, "CONFIGURATION", 13)) {
            sscanf (line+13, "%d", &status);

		} else if (!strnicmp (line, "EVA", 3)) {

			EVA_IP = true;

			}else {
            vessel->ParseScenarioLine (line, vs);
        }
    }
	switch (status) {
	case 0:
		stage=0;
		SetLmVesselDockStage(vessel);
		break;
	case 1:
		stage=1;
		SetLmVesselHoverStage(vessel);
		if (EVA_IP){
		SetupEVA(vessel);
		}
		break;
	case 2:
		stage=2;
		SetLmAscentHoverStage(vessel);
		break;
	case 3:
		SetLmAscentDockedStage(vessel);
		stage=3;
		break;

	}


}

DLLCLBK void ovcSetClassCaps (VESSEL *vessel, FILEHANDLE cfg)
{
SetLmVesselDockStage(vessel);
vessel->SetFuelMass(8165);
}


DLLCLBK void ovcSaveState (VESSEL *vessel, FILEHANDLE scn)

{
	vessel->SaveDefaultState (scn);
	oapiWriteScenario_int (scn, "CONFIGURATION", status);
	if (EVA_IP){
	oapiWriteScenario_int (scn, "EVA", int(TO_EVA));
	}


}