/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005



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
  *	Revision 1.16  2005/06/06 12:10:21  tschachim
  *	New SRF_ defines
  *	
  *	Revision 1.15  2005/05/26 16:00:31  tschachim
  *	New SRF constants, new physics constant
  *	
  *	Revision 1.14  2005/05/18 23:34:23  movieman523
  *	Added roughly correct masses for the various Saturn payloads.
  *	
  *	Revision 1.13  2005/05/12 00:16:51  movieman523
  *	Updated Apollo 13 explosion time.
  *	
  *	Revision 1.12  2005/05/05 21:38:51  tschachim
  *	some new constants
  *	
  *	Revision 1.11  2005/04/22 14:02:25  tschachim
  *	Surface defines
  *	Systems constants
  *	
  *	Revision 1.10  2005/04/14 23:15:22  movieman523
  *	Increased version number to 6.5.
  *	
  *	Revision 1.9  2005/04/01 14:12:40  tschachim
  *	nsurf = 28
  *	
  *	Revision 1.8  2005/03/28 05:48:42  chode99
  *	Added new payload type (Apollo 7 COAS target).
  *	
  *	Revision 1.7  2005/03/27 03:37:56  chode99
  *	Support for new type of test article payload (Apollo 8).
  *	
  *	Revision 1.6  2005/03/09 22:01:31  tschachim
  *	nsurf = 27
  *	
  *	Revision 1.5  2005/03/03 17:54:58  tschachim
  *	new font
  *	
  *	Revision 1.4  2005/02/24 00:10:05  movieman523
  *	Updated to version 6.4 and added extra digit to support more sub-versions up to x.x.9
  *	
  *	Revision 1.3  2005/02/19 00:01:57  movieman523
  *	Added more comments and course correction time setting.
  *	
  *	Revision 1.2  2005/02/12 13:27:03  movieman523
  *	Updated version number
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

#define NASSP_VERSION 60050
#define REALISM_DEFAULT 5

typedef struct {
	HINSTANCE hDLL;
	HFONT font[3];
	DWORD col[6];
	HBRUSH brush[4];
	HPEN pen[5];
} GDIParams;

//
// Control stages.
//

#define ROLLOUT_STAGE				1
#define PRELAUNCH_STAGE				5

#define LAUNCH_STAGE_ONE			11
#define LAUNCH_STAGE_TWO			12
#define LAUNCH_STAGE_TWO_ISTG_JET	13
#define LAUNCH_STAGE_TWO_TWR_JET	14

#define LAUNCH_STAGE_SIVB			20
#define STAGE_ORBIT_SIVB			21

#define CSM_LEM_STAGE				30

#define CM_STAGE					40
#define CM_ENTRY_STAGE				41
#define CM_ENTRY_STAGE_TWO			42
#define CM_ENTRY_STAGE_THREE		43
#define CM_ENTRY_STAGE_FOUR			44
#define CM_ENTRY_STAGE_FIVE			45
#define CM_ENTRY_STAGE_SIX			46
#define CM_ENTRY_STAGE_SEVEN		47
#define CM_RECOVERY_STAGE			48

#define CSM_ABORT_STAGE				50

//
// Start putting in defines rather than hard-coded numbers.
//

const int nsurf = 30; // number of bitmap handles

#define SRF_INDICATOR				 1
#define SRF_NEEDLE					 2
#define SRF_SWITCHUP				 6
#define SRF_SWITCHGUARDS			 8
#define SRF_ABORT					 9
#define SRF_SEQUENCERSWITCHES		17
#define SRF_DSKY					21
#define SRF_DSKYNUM					22
#define SRF_THREEPOSSWITCH			23
#define SRF_ROTATIONALSWITCH		27
#define SRF_SUITCABINDELTAPMETER	28
#define SRF_THREEPOSSWITCH305   	29

//
// Earth radius and gravity constants.
//

static const double ERADIUS = 6371.0;
const double GK    = 6.67259e-20 ;			//  Gravitational constant (km^3/(kg sec^2))
const double GKSI  = 6.67259e-20 * 1e9;
const double PSI   = 0.000145038;			// Pa to PSI
const double MMHG  = 0.00750064;			// Pa to mmHg
const double INH2O = 0.00401474;			// Pa to in H2O
const double LBH   = 7.93665;				// g/s to lb/h

inline double KelvinToFahrenheit(double kelvin) {
	return kelvin * 1.8 - 459.67;
}

//
// Engine information.
//

#define SPS_THRUST		100552.5
#define SPS_ISP			3778.5
#define APS_ISP			2921
#define APS_THRUST		15880
#define DPS_THRUST		44910
#define DPS_ISP			3107

//
// Mission times for specific events.
//

#define APOLLO_13_EXPLOSION_TIME	(55.0 * 3600 + 55.0 + 10.0)

//
// We include a sizeable overlap here for the course correction earliest and latest times,
// to allow for people using high time accelerations.
//

#define COURSE_CORRECTION_START_TIME	219400
#define COURSE_CORRECTION_END_TIME	(COURSE_CORRECTION_START_TIME + 10000)

//
// Flags to show the mesh is visible externally and in the virtual cockpit.
//

#define MESHVIS_VCEXTERNAL	(MESHVIS_EXTERNAL|MESHVIS_VC)

//
// SIVB payloads.
//

#define PAYLOAD_LEM		0
#define PAYLOAD_ASTP	1
#define PAYLOAD_LTA		2
#define PAYLOAD_LM1		3
#define PAYLOAD_LTA8	4
#define PAYLOAD_TARGET	5
#define PAYLOAD_LTA6	6

//
// Internal systems.
//

#define CSM_H2TANK_CAPACITY  12700.0		// in g, 28 lb
#define CSM_O2TANK_CAPACITY 145149.0		// in g, 320 lb

