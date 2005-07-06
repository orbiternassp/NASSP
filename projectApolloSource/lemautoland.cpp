/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Saturn LEM computer
  Autoland code based on LMMFD

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
  *	Revision 1.9  2005/06/17 18:26:14  lazyd
  *	Added GetHorizVelocity function for crosspointers etc.
  *	
  *	Revision 1.8  2005/06/15 15:52:01  lazyd
  *	Added DebugString to P64, P65 and P66 for left window panel
  *	
  *	Revision 1.7  2005/06/14 15:43:14  henryhallam
  *	*** empty log message ***
  *	
  *	Revision 1.6  2005/06/13 18:43:09  lazyd
  *	Added P66 and target redesignation to P64
  *	
  *	Revision 1.5  2005/06/09 14:16:31  lazyd
  *	Added code to turn off engine gimballing prior to powered descent
  *	
  *	Revision 1.4  2005/06/08 22:40:48  lazyd
  *	Using Emem 075 and 76 to save more lat/lon precision
  *	Final hover part of landing is much smoother
  *	
  *	Revision 1.3  2005/06/06 22:24:34  lazyd
  *	Save/Resore now works without any additional variables
  *	
  *	Revision 1.2  2005/06/04 20:23:43  lazyd
  *	Added code to land the LM on the moon
  *	
  *	Revision 1.1  2005/02/11 12:54:06  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"
#include "nasspdefs.h"

#include "apolloguidance.h"
#include "dsky.h"
#include "lemcomputer.h"
#include "sat5_lmpkd.h"

#define MAXOFFPLANE	44000
#define BRAKDIST	425000
#define PDIDIST		494000
//#define PDIDIST		504000
#define PDIDISTJ    508000
#define ULLAGESEC	7
#define OFFSET		60
#define DELTAT		2.0
#define LEADTIME	2.2
#define BRAKTIME	480.0
#define BRAKTIMEJ	540.0
#define LOCRIT      0.57
#define LOCRITJ     0.59369
#define HICRIT      0.63
#define HICRITJ		0.65618
#define MAXTHROT	0.93053
#define MAXTHROTJ   0.96895
#define A15DELCO	true
#define P64LOG		false
#define LOGFILE		false

//
// Code goes here
//


void LEMcomputer::Prog63(double simt)

{
	static VECTOR3 zero={0.0, 0.0, 0.0};
	static VECTOR3 xloc={1.0, 0.0, 0.0};
	static VECTOR3 yloc={0.0, 1.0, 0.0};
	static VECTOR3 zloc={0.0, 0.0, 1.0};
	//A11-A14 aim points
	static VECTOR3 rhii= {-3401.75, -959.49, 0.0};
	static VECTOR3 vhii= {    2.87, -60.45, 0.0};
	static VECTOR3 ahii= {   -2.2677, -0.221, 0.0};
	//quadratic guidance aim points from A15Delco...
	static VECTOR3 rhij= {-3612.75, -959.49, 0.0};
	static VECTOR3 vhij= {  -51.308, -60.45, 0.0};
	static VECTOR3 ahij= {   -2.554, -0.221, 0.0};
	const double GRAVITY=6.67259e-11;
	int nit;
	VECTOR3 vel, actatt, tgtatt, relpos, relvel, plvec, vnorm, zerogl, xgl, ygl, zgl, 
		upproj, lrproj, position, velocity, acc, rhi, vhi, ahi;
	double vlat, vlon, vrad, prograde, dlat, dlon, aa, cc, sbdis, tbrg, cbrg, rbrg, hvel,
		blat, blon, tgo, centrip, grav, cthrust, accx, acctot, vmass, vthrust, pdidis, 
		maxacc, down, ttg, ttg2, ttg3, ttg4, ttgl, ttgl2, dtg, jfz, heading, maxthr, 
		locr, hicr, braktim;
	// go through guidance calcs every 2 seconds
//	tgo=-540.0-(BurnStartTime-simt);
	if(simt > NextEventTime) {
//		sprintf(oapiDebugString(),"ProgState= %d  simt=%.1f ", ProgState, simt);
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		double bradius=oapiGetSize(hbody);
		double bmass=oapiGetMass(hbody);
		if(ProgState >=0) {
		//do the guidance calculations here...
			if(ProgState != 6) NextEventTime=simt+DELTAT;
			OurVessel->GetHorizonAirspeedVector(vel);
			OurVessel->GetEquPos(vlon, vlat, vrad);
			// used for altitude rate * 10
			DesiredApogee=vel.y*10.0;
			CurrentAlt=vrad-bradius;
			if(vel.x < 0) {
				prograde=-1;
			} else {
				prograde=1;
			}
// spherical calcs
			blat=LandingLatitude*RAD;
			blon=LandingLongitude*RAD;
			dlat=vlat-blat;
			dlon=vlon-blon;
			aa = pow(sin(dlat/2), 2) + cos(blat) * cos (vlat) * pow(sin(dlon/2), 2);
			cc = 2 * atan2(sqrt(aa), sqrt(1 - aa));
			sbdis=bradius*cc;
			//Downrange distance for noun 68
			DeltaPitchRate=sbdis;
			if(ProgState == 0) return;
	//sbdis is spherical distance to base position
			hvel=sqrt(vel.x*vel.x+vel.z*vel.z);
			if(ProgState == 6) CutOffVel=hvel;
			CurrentVel=hvel;
			CurrentRVel=CutOffVel-hvel;
			if(ApolloNo > 14) {
				//A15Delco
				jfz=0.004652;
				rhi=rhij;
				vhi=vhij;
				ahi=ahij;
				braktim=BRAKTIMEJ;
				pdidis=PDIDISTJ;
				maxthr=MAXTHROTJ;
				locr=LOCRITJ;
				hicr=HICRITJ;
			} else {
				//A11-14
				jfz=0.0101568;
				rhi=rhii;
				vhi=vhii;
				ahi=ahii;
				braktim=BRAKTIME;
				pdidis=PDIDIST;
				maxthr=MAXTHROT;
				locr=LOCRIT;
				hicr=HICRIT;
			}
			tgo=-braktim-OFFSET-(BurnStartTime-simt);
			BurnTime=braktim;
			if(sbdis > BRAKDIST) BurnStartTime=simt+(sbdis-pdidis)/hvel;
			if(ProgState <= 4) BurnEndTime=BurnStartTime+BurnTime;
			tbrg = atan2(sin(vlon - blon) * cos(blat), 
					cos(vlat) * sin(blat) - sin(vlat) * 
					cos(blat) * cos(vlon - blon));

			if (tbrg > (2*PI)) {
				tbrg -= 2*PI;
			}
			cbrg=atan2(vel.x,vel.z);
			if(vel.x < 0.0) {
			//retrograde
				tbrg=2*PI-tbrg;
				cbrg+=2*PI;
			} else {
			//prograde
				tbrg=-tbrg;
			}
			rbrg=tbrg-cbrg;
			if(fabs(rbrg) > PI) {
				rbrg-=2*PI;
			}
			// actually crossrange distance...
			DesiredDeltaV=(-sin(rbrg)*sbdis)/100.;
		}
		if(ProgState > 4) {
		// now we need to calculate attitude...
			OurVessel->GetRelativePos(hbody, relpos);
			OurVessel->GetRelativeVel(hbody, relvel);
//			plvec=CrossProduct(relpos, relvel);
			// for face-down...
			plvec=CrossProduct(relvel, relpos);
			//plvec is normal to orbital plane...
			plvec=Normalize(plvec);
//			vnorm=relpos;
			if(ProgState < 15) {
			//for face-down
				vnorm=-relpos;
				down=1.0;
			} else {
				vnorm=relpos;
				down=-1.0;
			}
			// set up frame of reference for body we are orbiting
			vnorm=Normalize(vnorm);
			OurVessel->Local2Global(zero, zerogl);
			OurVessel->Local2Global(xloc, xgl);
			OurVessel->Local2Global(yloc, ygl);
			OurVessel->Local2Global(zloc, zgl);
			xgl=xgl-zerogl;
			ygl=ygl-zerogl;
			zgl=zgl-zerogl;
			upproj.x=vnorm*xgl;
			upproj.y=vnorm*ygl;
			upproj.z=vnorm*zgl;
			lrproj.x=prograde*(plvec*xgl);
			lrproj.y=prograde*(plvec*ygl);
			lrproj.z=prograde*(plvec*zgl);
			//calculate our actual attitude wrt body frame
			actatt.x=PI/2.0-asin(upproj.y);
			actatt.y=down*(-prograde)*asin(lrproj.z);
			actatt.z=down*prograde*asin(lrproj.y);

			CurrentVelX=-actatt.x*DEG*100.;
			CurrentVelY=actatt.y*DEG*100.;
			CurrentVelZ=actatt.z*DEG*100.;
//			sprintf(oapiDebugString(),"att= %.2f %.2f %.2f", actatt*DEG);
			tgtatt.x=PI/2.;
			tgtatt.y=0.0;
			tgtatt.z=0.0;
			position.x=-sbdis*cos(rbrg);
			position.y=CurrentAlt;
			position.z=-sin(rbrg)*sbdis;
			velocity.x=hvel*cos(rbrg);
			velocity.y=vel.y;
			velocity.z=-hvel*sin(rbrg);
			if(ProgState == 13) {
				tgo=-braktim;
				ProgFlag01=false;
				ProgState++;
			}
			if(ProgState > 13) {
				centrip=hvel*hvel/(bradius+position.y);
				grav=(GRAVITY*bmass)/((bradius+position.y)*(bradius+position.y));
				//do the quadratic guidance - compute acceleration vector

				if(ProgState == 15) {
					if(tgo > -354.0) OurVessel->SetAttitudeRotLevel(1,0.0);
					if(tgo > -350.0) {
						if(fabs(actatt.y) < 0.35) {
							ProgState++;
							ProgFlag02=true;
						}
					}
				}
// roll over at 3 min into PD
				if(ProgState == 14) {
					if(tgo > -360) {
						ProgState++;
						ProgFlag02=false;
						OurVessel->SetAttitudeRotLevel(1,-0.5);
					}
				}
// Try this from A15Delco...
				ttg=tgo;
//solve for ttg
				//calculate time-to-go
				for (nit=0; nit<8; nit++) {
					ttg2=ttg*ttg;
					ttg3=ttg2*ttg;

					dtg=-(jfz*ttg3+6.0*ahi.x*ttg2+(18.0*vhi.x+6.0*velocity.x)*ttg+
						24.0*(rhi.x-position.x))/(3.0*jfz*ttg2+12.0*ahi.x*ttg+
						18.0*vhi.x+6.0*velocity.x);
					if(fabs(dtg) < 0.01) break;
					ttg=ttg+dtg;
				}
//				sprintf(oapiDebugString(),"ttg=%.1f",ttg);
				BurnEndTime=simt-ttg-60.0;
				ttg2=ttg*ttg;
				if(A15DELCO) {
					ttgl=ttg+LEADTIME;
					ttgl2=ttgl*ttgl;
					ttg3=ttg2*ttg;
					ttg4=ttg3*ttg;
					acc.x=(-24.0*(ttgl/ttg3)+36.0*(ttgl2/ttg4))*(rhi.x-position.x)+
						(-18.0*(ttgl/ttg2)+24.0*(ttgl2/ttg3))*vhi.x+
						(-6.0*(ttgl/ttg2)+12.0*(ttgl2/ttg3))*velocity.x+
						(6.0*(ttgl2/ttg2)-6.0*(ttgl/ttg)+1.0)*ahi.x;
				
					acc.y=(-24.0*(ttgl/ttg3)+36.0*(ttgl2/ttg4))*(rhi.y-position.y)+
						(-18.0*(ttgl/ttg2)+24.0*(ttgl2/ttg3))*vhi.y+
						(-6.0*(ttgl/ttg2)+12.0*(ttgl2/ttg3))*velocity.y+
						(6.0*(ttgl2/ttg2)-6.0*(ttgl/ttg)+1.0)*ahi.y+grav-centrip;

					acc.z=(-24.0*(ttgl/ttg3)+36.0*(ttgl2/ttg4))*(rhi.z-position.z)+
						(-18.0*(ttgl/ttg2)+24.0*(ttgl2/ttg3))*vhi.z+
						(-6.0*(ttgl/ttg2)+12.0*(ttgl2/ttg3))*velocity.z+
						(6.0*(ttgl2/ttg2)-6.0*(ttgl/ttg)+1.0)*ahi.z;

				} else {
				
					acc.x=ahi.x+(6.0*(velocity.x+vhi.x))/ttg - (12.0*(position.x-rhi.x))/ttg2;
					acc.y=ahi.y+(6.0*(velocity.y+vhi.y))/ttg - (12.0*(position.y-rhi.y))/ttg2 - 
						centrip + grav;
					acc.z=ahi.z+(6.0*(velocity.z+vhi.z))/ttg - (12.0*(position.z-rhi.z))/ttg2;
				}

				acctot=Mag(acc);
				vmass=OurVessel->GetMass();
				vthrust=OurVessel->GetMaxThrust(ENGINE_HOVER);
//				sprintf(oapiDebugString(),"mass=%.1f %.1f %.1f %.1f", vmass, rhi);
				maxacc=MAXTHROT*vthrust/vmass;

				// limit z acceleration to yield < 15 degree roll
				double zfrac=acc.z/(maxacc);
				if(fabs(zfrac) > 0.2) {
					acc.z=0.2*(fabs(acc.z)/acc.z)*(maxacc);
					acctot=Mag(acc);
				}

				if(ProgFlag01) {
				// after throttledown...
					cthrust=acctot/maxacc;
					maxacc=maxacc*hicr;
					if (cthrust > hicr) {
						// can't throttle up after throttledown, let downrange take the slack
						accx=maxacc*maxacc-acc.y*acc.y-acc.z*acc.z;
						if(acc.x > 0.0) {
							acc.x=sqrt(fabs(accx));
						} else {
							acc.x=-sqrt(accx);
						}
						cthrust=hicr;
					}
				} else {
					// if quad is asking for more or less acceleration than we can do,
					// give Y and Z what they need and let X (downrange) take the slack,
					// we can hopefully make up for it during throttle recovery
					double ma=maxacc;
					accx=ma*ma-acc.y*acc.y-acc.z*acc.z;
					if(acc.x > 0.0) {
						acc.x=sqrt(fabs(accx));
					} else {
						acc.x=-sqrt(accx);
					}
					cthrust=maxthr;
				// before throttledown
					if ( acctot/maxacc <= locr) {
					//  set the throttledown flag to true
						ProgFlag01=true;
						cthrust=acctot/maxacc;
					}
				}
				acctot=Mag(acc);
//				sprintf(oapiDebugString()," acc=%.3f %.3f %.3f %.3f ", 
//					acc,acctot);
				// now let's convert the accelerations to attitude for the vessel...
				//face down...
				if(ProgState < 16) tgtatt.z=-asin(acc.z/acctot);
				//face up reverses roll...
				if(ProgState == 16) tgtatt.z=asin(acc.z/acctot);
				if(acc.x == 0.0) {
					tgtatt.x=0.0;
				} else {
					//face down...
					if(ProgState < 16) tgtatt.x=(PI/2.0+atan(acc.y/acc.x)*(fabs(acc.x)/acc.x));
					//face up...
					if(ProgState == 16) tgtatt.x=(PI/2.0-atan(acc.y/acc.x)*(fabs(acc.x)/acc.x));
				}
				tgtatt.y=0.0;
				OurVessel->SetEngineLevel(ENGINE_HOVER, cthrust);
//				sprintf(oapiDebugString(),
//					"acc=%.2f %.2f %.2f att=%.1f %.1f %.1f act=%.1f %.1f %.1f ath=%.3f tgo=%.1f",
//					acc, tgtatt*DEG, actatt*DEG, cthrust, ttg+60.);
			}
			if(ProgFlag02) ComAttitude(actatt, tgtatt);
		}
		//end of 2-second interval guidance calcs
	}
//	sprintf(oapiDebugString(),"ProgState, Time, Next= %d %.1f %.1f", ProgState, 
//			simt, NextEventTime);
	double dt=BurnStartTime-simt;
	switch (ProgState) {
	case 0:
		SetVerbNounAndFlash(6, 89);
		NextEventTime=simt;
		ProgState++;
		break;

// display time from ignition and crossrange..
	case 2:
		SetVerbNounAndFlash(6, 61);
		if(fabs(DesiredDeltaV) > MAXOFFPLANE) dsky.LightOprErr();
		break;

//display time of ignition hhmmssss
	case 3:
		BurnTime=BurnStartTime;
		SetVerbNounAndFlash(6, 33);
		break;

//display Roll, Pitch, and Yaw angles for automaneuver...
	case 4:
		SetVerbNounAndFlash(50, 18);
		break;

// Orient spacecraft retrograde and face-down for PDI
	case 5:
		SetVerbNoun(6,18);
		//Turn on Attitude control
		ProgFlag02=true;
//		sprintf(oapiDebugString(),"t=%.1f ", dt);
// 35 seconds from ignition, blank the DSKY for 5 seconds
		if(dt <= 35) {
			dsky.BlankData();
			NextEventTime=simt+5;
//			CutOffVel=hvel;
			ProgState++;
		}
		break;

	case 6:
//		sprintf(oapiDebugString(),"Next=%.1f simt=%.1f", NextEventTime, simt);
		if(simt >= NextEventTime) {
			ProgState++;
		}
		break;

	case 7:
		SetVerbNoun(6, 62);
		if(dt <= 7.0) {
			sat5_lmpkd *lem = (sat5_lmpkd *) OurVessel;
			lem->SetGimbal(false);
			OurVessel->SetAttitudeLinLevel(1,1.0);
			ProgState++;
		}
		break;

	case 8:
		SetVerbNoun(6,62);
		if(dt <= 6.0) ProgState++;
		break;

	case 9:
		SetVerbNounAndFlash(99, 62);
		break;

	case 10:
		SetVerbNoun(6,62);
		if(dt <= 0.0) {
			OurVessel->SetEngineLevel(ENGINE_HOVER, 0.1);
			ProgState++;
		}
		break;

	case 11:
		SetVerbNounAndFlash(6,63);
//		sprintf(oapiDebugString(),"Ignition 10%% ");
		if(dt <= -0.5) {
			OurVessel->SetAttitudeLinLevel(1, 0.0);
			ProgState++;
		}
		break;
		
	case 12:
		SetVerbNounAndFlash(6,63);
		if(dt <= -26.0) {
//			sprintf(oapiDebugString(),"Throttle Up");
			OurVessel->SetEngineLevel(ENGINE_HOVER, 1.0);
			NextEventTime=simt;
			ProgState++;
		}
		break;

	case 14:
		SetVerbNoun(6,63);
		break;

	case 16:
		SetVerbNoun(16,68);
		if(simt >= BurnEndTime) {
			NextEventTime=simt;
			ProgState=0;
			oapiGetHeading(OurVessel->GetHandle(), &heading);
			BurnTime=heading;
			RunProgram(64);
		}
		break;

	}


}

void LEMcomputer::Prog63Pressed(int R1, int R2, int R3)

{
//	sprintf(oapiDebugString(),"Pressed %d %d %d %d ",ProgState, R1, R2, R3);
	switch (ProgState)
	{
	//
	// 1: Getting landing site location.
	//
	case 1:
		NextEventTime=0;
		//check latitude between -90 and +90
		if (R1 > 90000 || R1 < -90000)
			break;
		//check longitude/2 between -90 and +90
		if (R2 > 90000 || R2 < -90000)
			break;
// don't mess with this for now...
//		LandingLatitude = (double) R1 / 1000.0;
//		LandingLongitude = (double) R2 / 500.0;
//		LandingAltitude = (double) R3;
//		sprintf(oapiDebugString(),"Lat= %.8f Lon=%.8f Alt=%.2f",
//			LandingLatitude, LandingLongitude, LandingAltitude);
		ProgState++;
		return;

	//
	// 2: PRO on burntime, time from ignition, crossrange
	//
	case 2:
		ProgState++;
		return;

	//
	// PRO on time of ignition...
	//
	case 3:
		CurrentVelX=(-PI/2.)*DEG*100.;
		CurrentVelY=0.;
		CurrentVelZ=0.;
		ProgState++;
		return;

	//
	//PRO on automaneuver
	//
	case 4:
		ProgState++;
		return;

	//
	//PRO for ignition
	//
	case 9:
		ProgState++;
		return;
	}

	dsky.LightOprErr();
}

void LEMcomputer::Prog64(double simt)
{
	//Position targets for low gate
	static VECTOR3 rlow= {-8.415, 48.76, 0.0};
	//Velocity targets for low gate
	static VECTOR3 vlow= {  -0.0077,  -1.086, 0.0};
	//Acceleration targets for low gate
	static VECTOR3 alow= {   -0.1812, 0.0237, 0.0};
	static VECTOR3 pdvec= { 0.5, 0.866, 0.0 };
	const double GRAVITY=6.67259e-11;
	int nit, lpd;
	VECTOR3 vel, actatt, tgtatt, acc, position, velocity;
	double vlon, vlat, vrad, prograde, blat, blon, dlat, dlon, aa, cc, sbdis, hvel, 
		tbrg, cbrg, rbrg, heading, jfz, ttg, ttg2, ttg3, ttg4, ttgl,
		ttgl2, dtg, grav, acctot, vmass, vthrust, maxacc, cthrust, accx, cgelev;
	if(ProgState==0) {
		if(P64LOG) {
			char fname[8];
			sprintf(fname,"P64log.txt");
			outstr=fopen(fname,"w");
			fprintf(outstr, "Approach mode beginning: \n");
		}

//        sat5_lmpkd *lem = (sat5_lmpkd *) OurVessel; 
//		OurVessel->SetThrusterDir(lem->th_hover[0], pdvec);
//		OurVessel->SetThrusterDir(lem->th_hover[1], pdvec);
//		OurVessel->GetThrusterDir(lem->th_hover[0],vel);
//		sprintf(oapiDebugString(),"hover  %.3f %.3f %.3f", vel);
		BurnStartTime=simt;
		BurnEndTime=BurnStartTime+100.0;
		ProgFlag01=false;
		ProgState++;
	}
	if(ProgFlag01) {
		sat5_lmpkd *lem = (sat5_lmpkd *) OurVessel;
		lem->SetGimbal(true);
		if (P64LOG) {
			fprintf(outstr, "Run P66: Close file.");
			fclose(outstr);
		}
		ProgState=0;
		OurVessel->GetHorizonAirspeedVector(vel);
		DesiredDeltaVy=vel.y;
		NextEventTime=simt;
		RunProgram(66);
		return;
	}

//	sprintf(oapiDebugString(),"state= %d", ProgState);
	if(simt > NextEventTime) {
		NextEventTime=simt+DELTAT;
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		double bradius=oapiGetSize(hbody);
		double bmass=oapiGetMass(hbody);
		OurVessel->GetHorizonAirspeedVector(vel);
		OurVessel->GetEquPos(vlon, vlat, vrad);
		actatt.x=OurVessel->GetPitch();
		actatt.z=-OurVessel->GetBank();
		oapiGetHeading(OurVessel->GetHandle(), &heading);
//		actatt.y=BurnTime-heading;
		DesiredApogee=vel.y*10.0;
		cgelev=OurVessel->GetCOG_elev();
		CurrentAlt=vrad-bradius-cgelev;
		if(vel.x < 0) {
			prograde=-1;
		} else {
			prograde=1;
		}
// spherical calcs
		blat=LandingLatitude*RAD;
		blon=LandingLongitude*RAD;
		dlat=vlat-blat;
		dlon=vlon-blon;
		aa = pow(sin(dlat/2), 2) + cos(blat) * cos (vlat) * pow(sin(dlon/2), 2);
		cc = 2 * atan2(sqrt(aa), sqrt(1 - aa));
		sbdis=bradius*cc;
	//sbdis is spherical distance to base position
		hvel=sqrt(vel.x*vel.x+vel.z*vel.z);
		CurrentVel=hvel;
//		CurrentRVel=CutOffVel-hvel;
		tbrg = atan2(sin(vlon - blon) * cos(blat), 
				cos(vlat) * sin(blat) - sin(vlat) * 
				cos(blat) * cos(vlon - blon));

		if (tbrg > (2*PI)) {
			tbrg -= 2*PI;
		}
		cbrg=atan2(vel.x,vel.z);
		if(vel.x < 0.0) {
		//retrograde
			tbrg=2*PI-tbrg;
			cbrg+=2*PI;
		} else {
		//prograde
			tbrg=-tbrg;
		}
		rbrg=tbrg-cbrg;
		if(fabs(rbrg) > PI) {
			rbrg-=2*PI;
		}
		actatt.y=tbrg-heading;
		BurnTime=heading;

		position.x=-sbdis*cos(tbrg-heading);
		position.y=CurrentAlt;
		position.z=sbdis*sin(tbrg-heading);
		velocity.x=hvel*cos(cbrg-heading);
		velocity.y=vel.y;
		velocity.z=-hvel*sin(cbrg-heading);
		grav=(GRAVITY*bmass)/((bradius+position.y)*(bradius+position.y));
//solve for time-to-go
		jfz=0.01328;
		//calculate time-to-go
		ttg=-(BurnEndTime-simt+30.0);
		for (nit=0; nit<8; nit++) {
			ttg2=ttg*ttg;
			ttg3=ttg2*ttg;

			dtg=-(jfz*ttg3+6.0*alow.x*ttg2+(18.0*vlow.x+6.0*velocity.x)*ttg+
				24.0*(rlow.x-position.x))/(3.0*jfz*ttg2+12.0*alow.x*ttg+
				18.0*vlow.x+6.0*velocity.x);
			if(fabs(dtg) < 0.01) break;
			ttg=ttg+dtg;
		}
//		if(ttg > -30.0) {
		if(ttg > -12.0) {
			if(P64LOG) {
				fprintf(outstr, "Start P65: Close file.");
				fclose(outstr);
			}
		//run P65
			ProgState=0;
			BurnTime=heading;
			RunProgram(65);
		}
		BurnEndTime=simt-ttg-10.0;
		ttg2=ttg*ttg;
//		sprintf(oapiDebugString(),"nit=%d  ttg=%.1f dtg=%.6f", nit,ttg,dtg);
		if(A15DELCO) {
		// this is how guidance accelerations are calculated in A15Delco...
			ttgl=ttg+LEADTIME;
			ttgl2=ttgl*ttgl;
			ttg3=ttg2*ttg;
			ttg4=ttg3*ttg;
			acc.x=(-24.0*(ttgl/ttg3)+36.0*(ttgl2/ttg4))*(rlow.x-position.x)+
				(-18.0*(ttgl/ttg2)+24.0*(ttgl2/ttg3))*vlow.x+
				(-6.0*(ttgl/ttg2)+12.0*(ttgl2/ttg3))*velocity.x+
				(6.0*(ttgl2/ttg2)-6.0*(ttgl/ttg)+1.0)*alow.x;
				
			acc.y=(-24.0*(ttgl/ttg3)+36.0*(ttgl2/ttg4))*(rlow.y-position.y)+
				(-18.0*(ttgl/ttg2)+24.0*(ttgl2/ttg3))*vlow.y+
				(-6.0*(ttgl/ttg2)+12.0*(ttgl2/ttg3))*velocity.y+
				(6.0*(ttgl2/ttg2)-6.0*(ttgl/ttg)+1.0)*alow.y+grav;

			acc.z=(-24.0*(ttgl/ttg3)+36.0*(ttgl2/ttg4))*(rlow.z-position.z)+
				(-18.0*(ttgl/ttg2)+24.0*(ttgl2/ttg3))*vlow.z+
				(-6.0*(ttgl/ttg2)+12.0*(ttgl2/ttg3))*velocity.z+
				(6.0*(ttgl2/ttg2)-6.0*(ttgl/ttg)+1.0)*alow.z;
		} else {
		// this is how accelerations are calculated in Luminary131
			acc.x=alow.x+(6.0*(velocity.x+vlow.x))/ttg - (12.0*(position.x-rlow.x))/ttg2;
			acc.y=alow.y+(6.0*(velocity.y+vlow.y))/ttg - (12.0*(position.y-rlow.y))/ttg2 + grav;
			acc.z=alow.z+(6.0*(velocity.z+vlow.z))/ttg - (12.0*(position.z-rlow.z))/ttg2;
		}
//
		acctot=Mag(acc);
		vmass=OurVessel->GetMass();
		vthrust=OurVessel->GetMaxThrust(ENGINE_HOVER);
		maxacc=vthrust/vmass;
		// after throttledown...
		cthrust=acctot/maxacc;
		maxacc=maxacc*0.6772;
		if (cthrust > 0.6772) {
			// can't throttle up after throttledown, let x take the slack
			accx=maxacc*maxacc-acc.y*acc.y-acc.z*acc.z;
			if(acc.x > 0.0) {
				acc.x=sqrt(fabs(accx));
			} else {
				acc.x=-sqrt(accx);
			}
			cthrust=0.6772;
		}
		tgtatt.z=-asin(acc.z/acctot);
		tgtatt.y=0.0;
		tgtatt.x=(PI/2.0-atan(acc.y/acc.x)*(fabs(acc.x)/acc.x));
		// the LPD time and LPD angle are calculated and put into CutOffVel for N64 display
//		int secs=(int)(-ttg-45);
		int secs=(int)(-ttg-25);
		if(secs < 0) secs=0;
		if(secs > 99) secs=99;
		lpd=(int)((atan(position.y/fabs(position.x))+actatt.x)*DEG+0.5);
		CutOffVel=1000.0*secs+lpd;
		sprintf(oapiDebugString(),"LPD time=%d  LPD angle=%d",secs, lpd);
		OurVessel->SetEngineLevel(ENGINE_HOVER, cthrust);
//		sprintf (oapiDebugString(), "LPD angle %.1f", (atan(position.y/fabs(position.x))+actatt.x)*DEG);
//		sprintf(oapiDebugString(),
//			"acc=%.2f %.2f %.2f att=%.1f %.1f %.1f act=%.1f %.1f %.1f ath=%.3f tgo=%.1f cut=%.1f",
//			acc, tgtatt*DEG, actatt*DEG, cthrust, ttg+60., CutOffVel);
		ComAttitude(actatt, tgtatt);
		if(P64LOG) {
			fprintf(outstr,"Time-to-go= %.1f\n",ttg);
			fprintf(outstr,"Target Lat=%.8f Lon=%.8f \n",blat*DEG, blon*DEG);
			fprintf(outstr,"Actual Lat=%.8f Lon=%.8f \n",vlat*DEG, vlon*DEG);
			fprintf(outstr,"cbrg: %.3f tbrg:%.3f rbrg:%.3f hdg:%.3f time:%.2f\n",
				cbrg*DEG, tbrg*DEG, rbrg*DEG, heading*DEG, simt);
			fprintf(outstr,"Pos: %.3f %.3f %.3f \n", position);
			fprintf(outstr,"Vel: %.3f %.3f %.3f \n", velocity);
		}

	}
//end of 2-sec guidance calcs
		SetVerbNounAndFlash(6, 64);

}

void LEMcomputer::Prog65(double simt)
{

	static VECTOR3 zero={0.0, 0.0, 0.0};
	VECTOR3 acc, tgtatt, actatt, vel, position, velocity;
	const double GRAVITY=6.67259e-11; // Gravitational constant
	double yrate, az, tts, ax, ttgz, acctot, Mass, heading, yinit, grav, 
		sbdis, rbrg, cbrg, tbrg, vlon, vlat, vrad, prograde, blat, blon, dlat, dlon, aa, 
		cc, hvel, cthrust, maxacc, vmass, vthrust, cgelev;

//	sprintf(oapiDebugString(),"ProgState=%d", ProgState);
	if(ProgState > 2) return;

	if(ProgState == 2) {
		RunProgram(68);
		return;
	}
	if(OurVessel->GroundContact()) {
		ProgState++;
		OurVessel->SetEngineLevel(ENGINE_HOVER,0.0);
		OurVessel->SetAttitudeRotLevel(zero);
		RunProgram(68);
		return;
	}
	if(ProgFlag01) {
		sat5_lmpkd *lem = (sat5_lmpkd *) OurVessel;
		lem->SetGimbal(true);
		if(LOGFILE) {
			fprintf(outstr, "P66: Close file.");
			fclose(outstr);
		}
		OurVessel->GetHorizonAirspeedVector(vel);
		DesiredDeltaVy=vel.y;
		NextEventTime=simt;
		RunProgram(66);
		return;
	}
	if(ProgState == 0) {
		ProgFlag01=false;
		if(LOGFILE) {
			char fname[8];
			sprintf(fname,"llog.txt");
			outstr=fopen(fname,"w");
			fprintf(outstr, "Landing mode beginning: \n");
		}
		ProgState++;
		NextEventTime=simt;
	}


	if (simt > NextEventTime) {

		NextEventTime=simt+DELTAT;
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		double bradius=oapiGetSize(hbody);
		double bmass=oapiGetMass(hbody);
		OurVessel->GetHorizonAirspeedVector(vel);
		OurVessel->GetEquPos(vlon, vlat, vrad);
		actatt.x=OurVessel->GetPitch();
		actatt.z=-OurVessel->GetBank();
		oapiGetHeading(OurVessel->GetHandle(), &heading);
		CutOffVel=-vel.x*10.0;
		actatt.y=BurnTime-heading;
		DesiredApogee=vel.y*10.0;
		cgelev=OurVessel->GetCOG_elev();
		CurrentAlt=vrad-bradius-cgelev;
		if (CurrentAlt <= 0.0 ) {
			OurVessel->SetEngineLevel(ENGINE_HOVER,0.0);
			OurVessel->SetAttitudeRotLevel(zero);
			ProgState++;
			if (LOGFILE) {
				fprintf(outstr,"Actual Lat=%.8f Lon=%.8f \n",vlat*DEG, vlon*DEG);
				fprintf(outstr,"Target Lat=%.8f Lon=%.8f \n",
					LandingLatitude, LandingLongitude);
				fprintf(outstr, "Contact: Close file.");
				fclose(outstr);
			}
//			sprintf(oapiDebugString(),"lat=%.6f %.6f lon=%.6f %.6f",
//				LandingLatitude, vlat*DEG, LandingLongitude, vlon*DEG);
//			RunProgram(68);
			return;
		}
		if(vel.x < 0) {
			prograde=-1;
		} else {
			prograde=1;
		}
// spherical calcs
		blat=LandingLatitude*RAD;
		blon=LandingLongitude*RAD;
		dlat=vlat-blat;
		dlon=vlon-blon;
		aa = pow(sin(dlat/2), 2) + cos(blat) * cos (vlat) * pow(sin(dlon/2), 2);
		cc = 2 * atan2(sqrt(aa), sqrt(1 - aa));
		sbdis=bradius*cc;
	//sbdis is spherical distance to base position
		hvel=sqrt(vel.x*vel.x+vel.z*vel.z);
		CurrentVel=hvel;
		CurrentRVel=CutOffVel-hvel;
		tbrg = atan2(sin(vlon - blon) * cos(blat), 
				cos(vlat) * sin(blat) - sin(vlat) * 
				cos(blat) * cos(vlon - blon));

		if (tbrg > (2*PI)) {
			tbrg -= 2*PI;
		}
		cbrg=atan2(vel.x,vel.z);
		if(vel.x < 0.0) {
		//retrograde
			tbrg=2*PI-tbrg;
			cbrg+=2*PI;
		} else {
		//prograde
			tbrg=-tbrg;
		}
		rbrg=tbrg-cbrg;
		if(fabs(rbrg) > PI) {
			rbrg-=2*PI;
		}
		position.x=-sbdis*cos(tbrg-heading);
		position.y=CurrentAlt;
		position.z=sbdis*sin(tbrg-heading);
		velocity.x=hvel*cos(cbrg-heading);
		velocity.y=vel.y;
		velocity.z=-hvel*sin(cbrg-heading);
		sprintf(oapiDebugString(),"Forward velocity=%.1f Descent rate=%.1f Altitude=%.0f",
			velocity.x, vel.y, CurrentAlt);
		if(LOGFILE) {
			fprintf(outstr,"Target Lat=%.8f Lon=%.8f \n",blat*DEG, blon*DEG);
			fprintf(outstr,"Actual Lat=%.8f Lon=%.8f \n",vlat*DEG, vlon*DEG);
			fprintf(outstr,"cbrg: %.3f tbrg:%.3f rbrg:%.3f hdg:%.3f time:%.2f\n",
				cbrg*DEG, tbrg*DEG, rbrg*DEG, heading*DEG, simt);
			fprintf(outstr,"Pos: %.3f %.3f %.3f \n", position);
			fprintf(outstr,"Vel: %.3f %.3f %.3f \n", velocity);
		}


		Mass=OurVessel->GetMass();
		grav=(GRAVITY*bmass)/((bradius+position.y)*(bradius+position.y));

//		yinit=100;
		yinit=60;
//X logic...
		ax=(fabs(velocity.x)*velocity.x)/(2*(position.x));
		if(fabs(position.x) < 1.0) {
			if(fabs(ax) > 0.02) ax=0.0;
		}
		if(position.x > 0) ax=-ax;
		tts=position.x/velocity.x;
		if(tts > 0) {
			ax=-position.x/(50.0*DELTAT);
		}
		ax=ax*1.4;
		acc.x=ax;


//Z logic
		az=(fabs(velocity.z)*velocity.z)/(2*(position.z));
		if(position.z > 0) az=-az;
		ttgz=position.z/velocity.z;
		if(ttgz > 0) {
			az=-position.z/100;
		}
		az=az*1.3;
		acc.z=az;
// Y logic
//this works very well landing from low gate in 62 seconds or so
/*		yrate=-0.5+(position.y/yinit)*(-2.5);
		if(position.y < 20.0) yrate=-1.0;
		if(position.y < 10.0) yrate=-(position.y+10.0)/20.0;
		if(velocity.y > -0.5) {
			yrate=-0.5;
		}
		//ax=ax*1.5
*/
		double curve=(yinit-position.y)/yinit;
//		yrate=-3.5+(curve*curve*3.0);
		yrate=-2.0+(curve*curve*1.5);
		acc.y=grav+((yrate-velocity.y)/5.0)*DELTAT;
		



// end of landing mode guidance...
		acctot=sqrt(acc.x*acc.x+acc.y*acc.y+acc.z*acc.z);
		if(acc.x == 0.0) {
		//prevent zerodivide..
			tgtatt.x=0;
		} else {
			//convert y and x accerations to pitch angle
			if (acc.x < 0.0) {
				tgtatt.x=atan(acc.y/acc.x)+PI/2;
			} else {
				tgtatt.x=atan(acc.y/acc.x)-PI/2;;
			}
		}
		vmass=OurVessel->GetMass();
		vthrust=OurVessel->GetMaxThrust(ENGINE_HOVER);
		maxacc=vthrust/vmass;

		cthrust=acctot/maxacc;
		if(cthrust > 0.5) cthrust=0.5;

		tgtatt.z=-asin(acc.z/acctot);
		tgtatt.y=0.0;
		OurVessel->SetEngineLevel(ENGINE_HOVER, cthrust);
		if(LOGFILE) {
			fprintf(outstr,"acc: %.6f %.6f %.6f \n", acc);
			fprintf(outstr,"tgt: %.3f %.3f %.3f \n", tgtatt*DEG);
			fprintf(outstr,"act: %.3f %.3f %.3f \n", actatt*DEG);
			fprintf(outstr,"thr: %.3f yrate: %.3f\n", cthrust*100., yrate);
		}
			
//		sprintf(oapiDebugString(),
//			"acc=%.2f %.2f %.2f att=%.1f %.1f %.1f act=%.1f %.1f %.1f ath=%.3f ",
//			acc, tgtatt*DEG, actatt*DEG, cthrust);
		ComAttitude(actatt, tgtatt);
//		sprintf(oapiDebugString(),
//		"pos= %.2f %.2f %.2f vel= %.2f %.2f %.2f acc=%.2f %.2f %.2f att=%.2f %.2f %.2f th=%.1f",
//			position,velocity, acc, tgtatt*DEG, cthrust*100.);
	}
		SetVerbNounAndFlash(6, 60);
	
}


void LEMcomputer::Prog66(double simt)
{
	static VECTOR3 zero={0.0, 0.0, 0.0};
	const double GRAVITY=6.67259e-11; // Gravitational constant
	double vmass, vthrust, maxacc, cthrust, acctot, actattx, actattz, grav, 
		vlon, vlat, vrad, cgelev, accy, heading, hvel, cbrg;
	VECTOR3 velocity;
	if(ProgState > 1) return;

	if(ProgState == 1) {
		RunProgram(68);
		return;
	}
	if(simt > NextEventTime) {
		NextEventTime=simt+0.5;
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		double bradius=oapiGetSize(hbody);
		double bmass=oapiGetMass(hbody);
		OurVessel->GetEquPos(vlon, vlat, vrad);
		OurVessel->GetHorizonAirspeedVector(velocity);
		cgelev=OurVessel->GetCOG_elev();
		CurrentAlt=vrad-bradius-cgelev;
		if (CurrentAlt <= 0.0 ) {
			OurVessel->SetEngineLevel(ENGINE_HOVER,0.0);
			OurVessel->SetAttitudeRotLevel(zero);
			ProgState++;
		}
		DesiredApogee=velocity.y*10.0;
		CutOffVel=velocity.x;
		//
			oapiGetHeading(OurVessel->GetHandle(), &heading);
			hvel=sqrt(velocity.x*velocity.x+velocity.z*velocity.z);
			cbrg=atan2(velocity.x,velocity.z);
			if(velocity.x < 0.0) {
			//retrograde
				cbrg+=2*PI;
			} 
			sprintf(oapiDebugString(),"Forward velocity=%.1f Descent rate=%.1f Altitude=%.0f",
				(hvel*cos(cbrg-heading)), velocity.y, CurrentAlt);
		//
		grav=(GRAVITY*bmass)/((bradius+CurrentAlt)*(bradius+CurrentAlt));
		actattx=OurVessel->GetPitch();
		actattz=-OurVessel->GetBank();
		accy=grav+((DesiredDeltaVy-velocity.y)/2.0);
		vmass=OurVessel->GetMass();
		vthrust=OurVessel->GetMaxThrust(ENGINE_HOVER);
		maxacc=vthrust/vmass;
		acctot=accy/(cos(actattx)*cos(actattz));
		cthrust=acctot/maxacc;
		if(cthrust > 0.5) cthrust=0.5;
//		sprintf(oapiDebugString(),"acc=%.3f tot=%.3f thr=%.1f ddy=%.3f dy=%.3f",
//			accy,acctot, cthrust*100, DesiredDeltaVy, velocity.y);
		OurVessel->SetEngineLevel(ENGINE_HOVER, cthrust);
	}
	SetVerbNounAndFlash(6, 60);
}

void LEMcomputer::ChangeDescentRate(double delta) 
{
	static VECTOR3 zero={0.0, 0.0, 0.0};
	VECTOR3 vel;
	if(ProgFlag01 == false) {
		OurVessel->SetAttitudeRotLevel(zero);
		ProgFlag01=true;
		OurVessel->GetHorizonAirspeedVector(vel);
		DesiredDeltaVy=vel.y;
		return;
	}
	DesiredDeltaVy+=delta;
//	sprintf(oapiDebugString(),"Descent rate=%.3f", DesiredDeltaVy);
}

void LEMcomputer::RedesignateTarget(int axis, double direction)
{
	double xoffset, zoffset, vlon, vlat, vrad, blat, blon, dlat, dlon, aa, cc, distance, 
		alpd, cgelev;
	if(ProgRunning != 64) return;
	OBJHANDLE hbody=OurVessel->GetGravityRef();
	double bradius=oapiGetSize(hbody);
	// if there is no LPD time remaining, do nothing...
	if(CutOffVel < 1000.0) return;
	OurVessel->GetEquPos(vlon, vlat, vrad);
	cgelev=OurVessel->GetCOG_elev();
	CurrentAlt=vrad-bradius-cgelev;
// spherical calcs
	blat=LandingLatitude*RAD;
	blon=LandingLongitude*RAD;
	dlat=vlat-blat;
	dlon=vlon-blon;
	aa = pow(sin(dlat/2), 2) + cos(blat) * cos (vlat) * pow(sin(dlon/2), 2);
	cc = 2 * atan2(sqrt(aa), sqrt(1 - aa));
	distance=bradius*cc;
	if(axis == 0) {
	// move the landing site close or further 1/2 degree
		alpd=atan(CurrentAlt/fabs(distance));
		xoffset=-direction*(distance-(CurrentAlt/tan(alpd+0.5*RAD)));
		zoffset=0.0;
	} else {
	// move the landing site left or right 2 degrees
		//yaw our vessel so we are pointing to the site..
		BurnTime=BurnTime-direction*2.0*RAD;
		zoffset=direction*(distance/30.0);
		xoffset=0.0;
	}
	//now update our landing latitude and longitude
	dlat=(zoffset/bradius)*sin(BurnTime)+
		(xoffset/bradius)*cos(BurnTime);
	dlon=-(zoffset/(bradius*cos(blat)))*cos(BurnTime)+
		(xoffset/(bradius*cos(blat)))*sin(BurnTime);
	if(P64LOG) {
		fprintf(outstr,"Redesignate x=%.1f z=%.1f dlat=%.8f dlon=%.8f hdg=%.2f\n", 
			xoffset, zoffset, dlat*DEG, dlon*DEG, BurnTime*DEG);
	}
	LandingLatitude=(blat+dlat)*DEG;
	LandingLongitude=(blon+dlon)*DEG;
//	sprintf(oapiDebugString(),"New Lat=%.8f Lon=%.8f xoff=%.1f zoff=%.1f", 
//		LandingLatitude, LandingLongitude, xoffset, zoffset);
}
void LEMcomputer::GetHorizVelocity(double &forward, double &lateral)
{
		VECTOR3 velocity;
		double vlat, vlon, vrad, heading, hvel, cbrg;
		forward=0.0;
		lateral=0.0;
		if(ProgRunning < 64) return;
		if(ProgRunning > 66) return;
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		double bradius=oapiGetSize(hbody);
		OurVessel->GetEquPos(vlon, vlat, vrad);
		OurVessel->GetHorizonAirspeedVector(velocity);
		oapiGetHeading(OurVessel->GetHandle(), &heading);
		hvel=sqrt(velocity.x*velocity.x+velocity.z*velocity.z);
		cbrg=atan2(velocity.x,velocity.z);
		if(velocity.x < 0.0) {
		//retrograde
			cbrg+=2*PI;
		} 
		forward=hvel*cos(cbrg-heading);
		lateral=hvel*sin(cbrg-heading);
}

// This is adapted from Chris Knestrick's Control.cpp, which wouldn't work right here
// The main differences are rates are a linear function of delta angle, rather than a 
// step function, and we do all three axes at once
void LEMcomputer :: ComAttitude(VECTOR3 &actatt, VECTOR3 &tgtatt)
{
	const double RATE_MAX = RAD*(8.0);
//	const double RATE_MAX = RAD*(10.0);
	const double DEADBAND_LOW = RAD*(0.01);
//	const double RATE_FINE = RAD*(0.005);
	const double RATE_FINE = RAD*(0.01);

	const double RATE_NULL = RAD*(0.0001);

	VECTOR3 PMI, Level, Drate, delatt, Rate;
	double Mass, Size, MaxThrust, Thrust, Rdead;

	VESSELSTATUS status2;
	OurVessel->GetStatus(status2);
	OurVessel->GetPMI(PMI);
	Mass=OurVessel->GetMass();
	Size=OurVessel->GetSize();
	MaxThrust=OurVessel->GetMaxThrust(ENGINE_ATTITUDE);

	Rate.x=17.374;
	Rate.y=17.374;
	Rate.z=17.374;
	delatt=tgtatt - actatt;
//X axis
	Drate.x=17.374;
	if (fabs(delatt.x) < DEADBAND_LOW) {
		if(fabs(status2.vrot.x) < RATE_NULL) {
		// set level to zero
			Level.x=0;
		} else {
		// null the rate
			Thrust=-(Mass*PMI.x*status2.vrot.x)/Size;
			Level.x = min((Thrust/MaxThrust), 1);
		}
	} else {
		Rate.x=RATE_FINE+fabs(delatt.x)/3.0;
		if(Rate.x < RATE_FINE) Rate.x=RATE_FINE;
		if (Rate.x > RATE_MAX ) Rate.x=RATE_MAX;
		Rdead=min(Rate.x/2,RATE_FINE);
		if(delatt.x < 0) {
			Rate.x=-Rate.x;
			Rdead=-Rdead;
		}
		Drate.x=Rate.x-status2.vrot.x;
		Thrust=(Mass*PMI.x*Drate.x)/Size;
		if(delatt.x > 0) {
			if(Drate.x > Rdead) {
				Level.x=min((Thrust/MaxThrust),1);
			} else if (Drate.x < -Rdead) {
				Level.x=max((Thrust/MaxThrust),-1);
			} else {
				Level.x=0;
			}
		} else {
			if(Drate.x < Rdead) {
				Level.x=max((Thrust/MaxThrust),-1);
			} else if (Drate.x > -Rdead) {
				Level.x=min((Thrust/MaxThrust),1);
			} else {
				Level.x=0;
			}
		}
	}

//Y-axis
	Drate.y=17.374;
	if (fabs(delatt.y) < DEADBAND_LOW) {
		if(fabs(status2.vrot.y) < RATE_NULL) {
		// set level to zero
			Level.y=0;
		} else {
		// null the rate
			Thrust=-(Mass*PMI.y*status2.vrot.y)/Size;
			Level.y = min((Thrust/MaxThrust), 1);
		}
	} else {
		Rate.y=RATE_FINE+fabs(delatt.y)/3.0;
		if(Rate.y < RATE_FINE) Rate.y=RATE_FINE;
		if (Rate.y > RATE_MAX ) Rate.y=RATE_MAX;
		Rdead=min(Rate.y/2,RATE_FINE);
		if(delatt.y < 0) {
			Rate.y=-Rate.y;
			Rdead=-Rdead;
		}
		Drate.y=Rate.y-status2.vrot.y;
		Thrust=(Mass*PMI.y*Drate.y)/Size;
		if(delatt.y > 0) {
			if(Drate.y > Rdead) {
				Level.y=min((Thrust/MaxThrust),1);
			} else if (Drate.y < -Rdead) {
				Level.y=max((Thrust/MaxThrust),-1);
			} else {
				Level.y=0;
			}
		} else {
			if(Drate.y < Rdead) {
				Level.y=max((Thrust/MaxThrust),-1);
			} else if (Drate.y > -Rdead) {
				Level.y=min((Thrust/MaxThrust),1);
			} else {
				Level.y=0;
			}
		}
	}
//Z axis
	Drate.z=17.374;
	if (fabs(delatt.z) < DEADBAND_LOW) {
		if(fabs(status2.vrot.z) < RATE_NULL) {
		// set level to zero
			Level.z=0;
		} else {
		// null the rate
			Thrust=-(Mass*PMI.z*status2.vrot.z)/Size;
			Level.z = min((Thrust/MaxThrust), 1);
		}
	} else {
		Rate.z=RATE_FINE+fabs(delatt.z)/3.0;
		if(Rate.z < RATE_FINE) Rate.z=RATE_FINE;
		if (Rate.z > RATE_MAX ) Rate.z=RATE_MAX;
		Rdead=min(Rate.z/2,RATE_FINE);
		if(delatt.z< 0) {
			Rate.z=-Rate.z;
			Rdead=-Rdead;
		}
		Drate.z=Rate.z-status2.vrot.z;
		Thrust=(Mass*PMI.z*Drate.z)/Size;
		if(delatt.z > 0) {
			if(Drate.z > Rdead) {
				Level.z=min((Thrust/MaxThrust),1);
			} else if (Drate.z < -Rdead) {
				Level.z=max((Thrust/MaxThrust),-1);
			} else {
				Level.z=0;
			}
		} else {
			if(Drate.z < Rdead) {
				Level.z=max((Thrust/MaxThrust),-1);
			} else if (Drate.z > -Rdead) {
				Level.z=min((Thrust/MaxThrust),1);
			} else {
				Level.z=0;
			}
		}
	}
	OurVessel->SetAttitudeRotLevel(Level);
}

