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

  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "soundlib.h"
#include "nasspdefs.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "lemcomputer.h"
#include "IMU.h"

#include "LEM.h"
#include "papi.h"

#include "CollisionSDK/CollisionSDK.h"

#define MAXOFFPLANE		44000
#define BRAKDIST		425000
#define PDIDIST			494000
#define PDIDISTJ		508000
#define ULLAGESEC		7
#define OFFSET			60
#define DELTAT			2.0
#define DELTAT_P36		0.1

#define LEADTIME		2.2
#define BRAKTIME		480.0
#define BRAKTIMEJ		540.0
#define LOCRIT			0.57
#define LOCRITJ			0.59369
#define HICRIT			0.63
#define HICRITJ			0.65618
#define MAXTHROT		0.95868	//0.93053
#define MAXTHROTJ		0.98531 //0.96895
#define A15DELCO		true
#define ABORTLOG		true
#define P64LOG			false
#define LOGFILE			false
#define TRTHRUST		1780.0
#define TRTHRUST_P36	50000.0
#define MINORBIT		18461.15
#define A14ASCENT		14

//
// Code goes here
//


void LEMcomputer::Prog63(double simt)

{
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
		upproj, lrproj, position, velocity, acc, rhi, vhi, ahi, accvec;
	double vlat, vlon, vrad, prograde, dlat, dlon, aa, cc, sbdis, tbrg, cbrg, rbrg, hvel,
		blat, blon, tgo, centrip, grav, cthrust, accx, acctot, vmass, vthrust, pdidis, 
		maxacc, down, ttg, ttg2, ttg3, ttg4, ttgl, ttgl2, dtg, jfz, heading, maxthr, 
		locr, hicr, braktim, sgn;
	// go through guidance calcs every 2 seconds
	LightCompActy();
	if(simt > NextEventTime) {
//		sprintf(oapiDebugString(),"ProgState= %d  simt=%.1f ", ProgState, simt);
		if(ProgState == 0) {
// MeshLand change..
			char pname[5];
			oapiGetObjectName(OurVessel->GetSurfaceRef(),pname,5); 		
			LandingAltitude=VSGetAbsMaxElvLoc(pname, LandingLatitude*RAD, LandingLongitude*RAD);
			LightAlt();
			LightVel();
//			sprintf(oapiDebugString(),"Alt Vel lights on");
		}
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
			if(ProgState >= 16) {
				if(CurrentAlt <= 13000.0) {
					ClearAlt();
					ClearVel();
//					sprintf(oapiDebugString(),"Alt Vel lights out");
				}
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
//			sprintf(oapiDebugString(), "distance=%.1f", sbdis);
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
			if(sbdis > BRAKDIST) BurnStartTime=simt+(sbdis-pdidis)/hvel;
			if(ProgState <= 4) {
				BurnEndTime=BurnStartTime+braktim;
//				BurnTime=BurnStartTime;
			}
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
			accvec=_V(-1.0, 0.0, 0.0);
		// now we need to calculate attitude...
			OurVessel->GetRelativePos(hbody, relpos);
			OurVessel->GetRelativeVel(hbody, relvel);
			// for face-down...
			plvec=CrossProduct(relvel, relpos);
			//plvec is normal to orbital plane...
			plvec=Normalize(plvec);
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
			OurVessel->Local2Global(_V(0.0, 0.0, 0.0), zerogl);
			OurVessel->Local2Global(_V(1.0, 0.0, 0.0), xgl);
			OurVessel->Local2Global(_V(0.0, 1.0, 0.0), ygl);
			OurVessel->Local2Global(_V(0.0, 0.0, 1.0), zgl);
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
//			actatt.y=down*(-prograde)*asin(lrproj.z)*acos(upproj.z);
			actatt.z=down*prograde*asin(lrproj.y);

			CurrentPitch=-actatt.x*DEG*100.;
			CurrentRoll=actatt.y*DEG*100.;
			CurrentYaw=actatt.z*DEG*100.;
//			sprintf(oapiDebugString(),"att= %.2f %.2f %.2f", actatt*DEG);
			tgtatt.x=PI/2.;
			tgtatt.y=0.0;
			tgtatt.z=0.0;
			position.x=-sbdis*cos(rbrg);
			position.y=CurrentAlt-LandingAltitude;
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
				ProgFlag03=false;
				if(ProgState == 15) {
//					if(tgo > -354.0) OurVessel->SetAttitudeRotLevel(1,0.0); //for old RCS thrust
					if(tgo > -358.0) OurVessel->SetAttitudeRotLevel(1,0.0);
					if(tgo > -356.0) {
						if((upproj.z > 0.0) && (fabs(actatt.y) < 0.25)) {
							ProgState++;
							ProgFlag02=true;
							ProgFlag03=true;
						}
					}
				}
// roll over at 3 min into PD
				if(ProgState == 14) {
					if(tgo > -360) {
						ProgState++;
						ProgFlag02=false;
						// yaw "the short way" to face-up
						sgn=lrproj.z/fabs(lrproj.z);
						OurVessel->SetAttitudeRotLevel(1,0.5*sgn);
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
				//sprintf(oapiDebugString(),"ttg=%.1f fabs(dtg)=%f",ttg,fabs(dtg));
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

//				sprintf(oapiDebugString(), "p=%.1f %.1f %.1f v=%.1f %.1f %.1f a=%.3f %.3f %.3f", position, velocity, acc);
				acctot=Mag(acc);
				vmass=OurVessel->GetMass();
				vthrust=OurVessel->GetMaxThrust(ENGINE_HOVER);
//				sprintf(oapiDebugString(),"mass=%.1f ", vmass);
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
				if(ProgState >= 16) tgtatt.z=asin(acc.z/acctot);
				if(acc.x == 0.0) {
					tgtatt.x=0.0;
				} else {
					//face down...
					if(ProgState < 16) tgtatt.x=(PI/2.0+atan(acc.y/acc.x)*(fabs(acc.x)/acc.x));
					//face up...
					if(ProgState >= 16) tgtatt.x=(PI/2.0-atan(acc.y/acc.x)*(fabs(acc.x)/acc.x));
				}
				accvec=Normalize(acc);
				tgtatt.y=0.0;
				OurVessel->SetEngineLevel(ENGINE_HOVER, cthrust);
				//sprintf(oapiDebugString(),
				//	"acc=%.2f %.2f %.2f att=%.1f %.1f %.1f act=%.1f %.1f %.1f ath=%.3f tgo=%.1f",
				//	acc, tgtatt*DEG, actatt*DEG, cthrust, ttg+60.);
			}
			if(ProgState < 15) {
				OrientAxis(accvec, 1, 1);
			} else {
				if(ProgFlag02) ComAttitude(actatt, tgtatt, ProgFlag03);
			}

			int current_mode=0;
			double timetoignition=0;
			double timesincepdi=0;


			current_mode = 0;
			if(ProgState > 13)
			{
				current_mode = 1;
				timesincepdi = simt-BurnStartTime;
				timetoapproach = BurnEndTime-simt;
			}
			else timetoignition = BurnStartTime-simt;



            SetStatus(simt,
                      current_mode,
				      timetoignition,
					  timesincepdi,
					  timetoapproach);					  		
        }
		//end of 2-second interval guidance calcs
	}
	//sprintf(oapiDebugString(),"ProgState, Time, Next= %d %.1f %.1f", ProgState, 
	//		simt, NextEventTime);
	double dt=BurnStartTime-simt;
	switch (ProgState) {
	case 0:
		SetVerbNounAndFlash(6, 89);
		NextEventTime=simt;
		ProgState++;
		if (Realism == 0) ProgState++;
		break;

// display time from ignition and crossrange..
	case 2:
		SetVerbNounAndFlash(6, 61);
		if (dt < 0 || fabs(DesiredDeltaV) > MAXOFFPLANE) {
			AbortWithError(01410);	// Guidance overflow, see A15Delco
			return;
		}
		if (Realism == 0) ProgState++;
		break;

//display time of ignition hhmmssss
	case 3:
		BurnTime=BurnStartTime;
		SetVerbNounAndFlash(6, 33);
		if (Realism == 0) ProgState++;
		break;

//display Roll, Pitch, and Yaw angles for automaneuver...
	case 4:
		SetVerbNounAndFlash(50, 18);
		if(Realism == 0) ProgState++;
		break;

// Orient spacecraft retrograde 
	case 5:
		SetVerbNoun(6,18);
		//Turn on Attitude control
		ProgFlag02=true;
		ProgFlag03=false;
		//sprintf(oapiDebugString(),"t=%.1f ", dt);
// 35 seconds from ignition, blank the DSKY for 5 seconds
		if(dt <= 35) {
			BlankAll();
			NextEventTime=simt+5;
//			CutOffVel=hvel;
			ProgState++;
		}
		break;

	case 6:
//		sprintf(oapiDebugString(),"Next=%.1f simt=%.1f", NextEventTime, simt);
		if(simt >= NextEventTime) {
			UnBlankAll();
			ProgState++;
		}
		break;

	case 7:
		SetVerbNoun(6, 62);
		if(dt <= 7.0) {
			LEM *lem = (LEM *) OurVessel;
			lem->SetGimbal(false);
			ProgState++;
		}
		break;

	case 8:
		SetVerbNoun(6,62);
		if(dt <= 6.0) ProgState++;
		break;

	case 9:
		SetVerbNounAndFlash(99, 62);
		if(Realism == 0) ProgState++;
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
			ProgState++;
		}
		break;
		
	case 12:
		SetVerbNounAndFlash(6,63);
		if(dt <= -26.0) {
//			sprintf(oapiDebugString(),"Throttle Up");
			OurVessel->SetEngineLevel(ENGINE_HOVER, 1.0);
			DesiredLAN=0.0;
			DesiredPlaneChange=simt;
			NextEventTime=simt;
			ProgState++;
		}
		break;

	case 14:
		SetVerbNoun(6,63);
		break;

	case 16:
		SetVerbNoun(16,68);
		ProgState++;
		break;

	case 17:
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
		CurrentPitch=(-PI/2.)*DEG*100.;
		CurrentRoll=0.;
		CurrentYaw=0.;
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

	LightOprErr();
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
	LightCompActy();
	if(ProgState==0) {
// MeshLand change..
		char pname[5];
		oapiGetObjectName(OurVessel->GetSurfaceRef(),pname,5); 		
		LandingAltitude=VSGetAbsMaxElvLoc(pname, LandingLatitude*RAD, LandingLongitude*RAD);
//		sprintf(oapiDebugString(), "lat=%.5f lon=%.5f alt=%.1f", LandingLatitude, LandingLongitude, LandingAltitude);
		if(P64LOG) {
			char fname[8];
			sprintf(fname,"P64log.txt");
			outstr=fopen(fname,"w");
			fprintf(outstr, "Approach mode beginning: \n");
		}

		BurnStartTime=simt;
		BurnEndTime=BurnStartTime+100.0;
		ProgFlag01=false;
		ProgState++;
	}
	if(ProgFlag01) {
		LEM *lem = (LEM *) OurVessel;
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
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		NextEventTime=simt+DELTAT;
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
		position.y=CurrentAlt-LandingAltitude;
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
		//sprintf(oapiDebugString(),"nit=%d  ttg=%.1f dtg=%.6f", nit,ttg,dtg);
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
//		tgtatt.y=0.0;
		tgtatt.x=(PI/2.0-atan(acc.y/acc.x)*(fabs(acc.x)/acc.x));
		// the LPD time and LPD angle are calculated and put into CutOffVel for N64 display
//		int secs=(int)(-ttg-45);
		int secs=(int)(-ttg-25);
		if(secs < 0) secs=0;
		if(secs > 99) secs=99;
		lpd=(int)((atan(position.y/fabs(position.x))+actatt.x)*DEG+0.5);
		tgtatt.y=-sin(actatt.z)*lpd*RAD;
		if(secs <=0.0) tgtatt.y=0.0;
		CutOffVel=1000.0*secs+lpd;
		sprintf(oapiDebugString(),"LPD time=%d  LPD angle=%d",secs, lpd);
//		sprintf(oapiDebugString(),"LPD time=%d  LPD angle=%d lat=%.6f, lon=%.6f alt=%.1f ",secs, lpd,
//			LandingLatitude, LandingLongitude, position.y);
		OurVessel->SetEngineLevel(ENGINE_HOVER, cthrust);
		ComAttitude(actatt, tgtatt, true);
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

		SetStatus(simt,
                           2,
		                   0,
			               0,
						   0);

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
	LightCompActy();
	if(ProgState > 2) return;

	if(ProgState == 2) {
		RunProgram(68);
		return;
	}

	double vsAlt = papiGetAltitude(OurVessel);
	if (OurVessel->GroundContact() || (vsAlt < 1.0)) {
		ProgState++;
		OurVessel->SetEngineLevel(ENGINE_HOVER,0.0);
		OurVessel->SetAttitudeRotLevel(zero);
		RunProgram(68);
		return;
	}
	if(ProgFlag01) {
		LEM *lem = (LEM *) OurVessel;
		lem->SetGimbal(true);
		if(LOGFILE) {
			fprintf(outstr, "P66: Close file.");
			fclose(outstr);
		}
		OurVessel->GetHorizonAirspeedVector(vel);
		DesiredDeltaVy=vel.y;
		NextEventTime=simt;
		ProgState=0;
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
		CurrentAlt = vsAlt;

		if (vsAlt < 1.0){ 

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
		position.y = vsAlt;
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
		ComAttitude(actatt, tgtatt, false);
//		sprintf(oapiDebugString(),
//		"pos= %.2f %.2f %.2f vel= %.2f %.2f %.2f acc=%.2f %.2f %.2f att=%.2f %.2f %.2f th=%.1f",
//			position,velocity, acc, tgtatt*DEG, cthrust*100.);
	}
		SetVerbNounAndFlash(6, 60);

		SetStatus(simt,
                           2,
		                   0,
			               0,
						   0);
	
}

void LEMcomputer::Prog66(double simt)
{
	static VECTOR3 zero={0.0, 0.0, 0.0};
	const double GRAVITY=6.67259e-11; // Gravitational constant
	double vmass, vthrust, maxacc, cthrust, acctot, actattx, actattz, grav, 
		vlon, vlat, vrad, cgelev, accy, heading, hvel, cbrg;
	VECTOR3 velocity;
	LightCompActy();
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

		double vsAlt = papiGetAltitude(OurVessel);
		CurrentAlt = vsAlt;

		if (OurVessel->GroundContact() || (vsAlt < 1.0)) { 
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
// MeshLand change..
	char pname[5];
	oapiGetObjectName(OurVessel->GetSurfaceRef(),pname,5); 		
	LandingAltitude=VSGetAbsMaxElvLoc(pname, LandingLatitude*RAD, LandingLongitude*RAD);
//	sprintf(oapiDebugString(), "lat=%.5f lon=%.5f alt=%.1f", LandingLatitude, LandingLongitude, LandingAltitude);
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

int LEMcomputer::GetProgRunning()
{
	return ProgRunning;
}

void LEMcomputer::Prog40(double simt)
{
	Prog42(simt);
}

void LEMcomputer::Prog41(double simt)
{
	const double GRAVITY=6.67259e-11;
	VECTOR3 dvdir, lmvel, lmpos;
	double dvmag, vmass;

	LightCompActy();
	if(simt > NextEventTime) {
		NextEventTime=simt + DELTAT;
		if (ProgState == 0) {
//			if(BurnStartTime-simt < 200.0) ProgState++;
			ProgFlag01=false;
			ProgFlag03=false;
//			OBJHANDLE hbody=OurVessel->GetGravityRef();
//			OurVessel->GetRelativeVel(hbody, lmvel);
			dvdir.x=DesiredDeltaVx;
			dvdir.y=DesiredDeltaVy;
			dvdir.z=DesiredDeltaVz;

			CommandedAttitudeRotLevel = _V(0, 0, 0);
			CommandedAttitudeLinLevel = _V(0, 0, 0);
		}
		if (ProgState >= 1) {
			dvdir.x=DesiredDeltaVx;
			dvdir.y=DesiredDeltaVy;
			dvdir.z=DesiredDeltaVz;
			CommandedAttitudeRotLevel = OrientAxis(dvdir, 1, 0);
		}
//		if(ProgFlag01 == false) sprintf(oapiDebugString(),"P41 burn=%.1f end=%.1f lvl=%.1f",
//			BurnStartTime-simt, BurnEndTime-simt, DesiredPlaneChange);
	}

	if (ProgFlag01) {
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		double bmass=oapiGetMass(hbody);
		OurVessel->GetRelativePos(hbody, lmpos);
		OurVessel->GetRelativeVel(hbody, lmvel);
		vmass=OurVessel->GetMass();
		if (simt >= BurnEndTime-0.05) {
			CommandedAttitudeRotLevel = _V(0, 0, 0);
			CommandedAttitudeLinLevel = _V(0, 0, 0);
			SetAttitudeRotLevel(_V(0, 0, 0));			
			oapiSetTimeAcceleration(DesiredDeltaV);
			ProgState=0;
			RunProgram((int) (DesiredLAN));
		}
		if (DesiredPlaneChange < 1.0 || ProgFlag03) {
			SetAttitudeRotLevel(CommandedAttitudeRotLevel);
			AddAttitudeLinLevel(CommandedAttitudeLinLevel);
			return;
		}
		if (simt >= BurnEndTime-0.5) {			
			CommandedAttitudeLinLevel.y = 0.1;			
			BurnEndTime=simt+(BurnEndTime-simt)*10.0;
			ProgFlag03=true;
		}
		SetAttitudeRotLevel(CommandedAttitudeRotLevel);
		AddAttitudeLinLevel(CommandedAttitudeLinLevel);
		return;
	}
	if (BurnStartTime-simt < 0.0) {
		// Turn off time acceleration
		DesiredDeltaV=oapiGetTimeAcceleration();
		oapiSetTimeAcceleration(1.0);
		// Turn on +Y thrusters
		CommandedAttitudeLinLevel.y = DesiredPlaneChange;		
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		OurVessel->GetRelativePos(hbody, lmpos);
		OurVessel->GetRelativeVel(hbody, lmvel);
		vmass=OurVessel->GetMass();
		dvdir.x=DesiredDeltaVx;
		dvdir.y=DesiredDeltaVy;
		dvdir.z=DesiredDeltaVz;
		dvmag=Mag(dvdir);
		CutOffVel=Mag(lmvel+dvdir);
		ProgFlag01=true;
	}
	SetAttitudeRotLevel(CommandedAttitudeRotLevel);
	AddAttitudeLinLevel(CommandedAttitudeLinLevel);

	if(ProgState == 0) {
//		SetVerbNounAndFlash(50, 18);
		SetVerbNoun(16, 31);
		ProgState++;
		return;
	}


	if(BurnStartTime-simt < 30.0) {	
		SetVerbNoun(16, 31);
		return;
	}

	if(BurnStartTime-simt < 30.2) {
		UnBlankAll();
		SetVerbNoun(16, 31);
		return;
	}

	if(BurnStartTime-simt < 35.0) {
		BlankAll();
		return;
	}
}

void LEMcomputer::Prog41Pressed(int R1, int R2, int R3)
{
	switch (ProgState)
	{
	case 0:
		SetVerbNounAndFlash(16, 18);
		ProgState++;
		return;
	}
	LightOprErr();
}

void LEMcomputer::Prog42(double simt)
{
	const double GRAVITY=6.67259e-11;
	VECTOR3 dvdir;
	double dvmag, vmass;

	LightCompActy();
	if(simt > NextEventTime) {
		NextEventTime=simt+DELTAT;
		if(ProgState == 0) {
			ProgFlag01=false;
			ProgFlag03=false;
			dvdir.x=DesiredDeltaVx;
			dvdir.y=DesiredDeltaVy;
			dvdir.z=DesiredDeltaVz;
		}
		if(ProgState >= 1) {
			dvdir.x=DesiredDeltaVx;
			dvdir.y=DesiredDeltaVy;
			dvdir.z=DesiredDeltaVz;
			OrientAxis(dvdir, 1, 0);
		}
	}
	if(ProgFlag01) {
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		double bmass=oapiGetMass(hbody);
		vmass=OurVessel->GetMass();
//		sprintf(oapiDebugString(), "time=%.1f vel=%.3f Cutoff=%.3f End=%.1f sim=%.1f", 
//			BurnStartTime-simt, Mag(lmvel), CutOffVel, BurnEndTime, simt);
		if(simt >= BurnEndTime-0.05) {
//			fprintf(outstr, "Pos after=%.1f %.1f %.1f off time=%.3f, mass=%.1f\n", 
//				lmpos, simt, vmass);
//			fprintf(outstr, "V after=%.3f %.3f %.3f time=%.3f mag=%.3f cut=%.3f\n", 
//				lmvel, simt, Mag(lmvel), CutOffVel);
			OurVessel->SetEngineLevel(ENGINE_HOVER, 0.0);
			oapiSetTimeAcceleration(DesiredDeltaV);
			ProgState=0;
			RunProgram((int) (DesiredLAN));
		}
		return;
	}
	if(BurnStartTime-simt < 0.0) {
		DesiredDeltaV=oapiGetTimeAcceleration();
		oapiSetTimeAcceleration(1.0);
		BurnEndTime=simt+(BurnEndTime-BurnStartTime);
		BurnStartTime=simt;
		// Turn on ascent engine
		OurVessel->SetEngineLevel(ENGINE_HOVER, 1.0);
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		vmass=OurVessel->GetMass();
		dvdir.x=DesiredDeltaVx;
		dvdir.y=DesiredDeltaVy;
		dvdir.z=DesiredDeltaVz;
		dvmag=Mag(dvdir);
		ProgFlag01=true;
	}

	if(ProgState == 0) {
		SetVerbNounAndFlash(50, 18);
		SetVerbNoun(16, 31);
		ProgState++;
		return;
	}

	if(BurnStartTime-simt < 30.0) {
		SetVerbNoun(16, 31);
		return;
	}

	if(BurnStartTime-simt < 30.2) {
		UnBlankAll();
		SetVerbNoun(16, 31);
		return;
	}

	if(BurnStartTime-simt < 35.0) {
		BlankAll();
		return;
	}


}

void LEMcomputer::Prog12(double simt)
{
	const double GRAVITY=6.67259e-11;
	double distance, radius, rlm, vel, time, cbrg, vh, csmapo, csmtta, csmper, csmttp, 
		csmperiod, a, mu, dapo, dper, dv, dh;
	VECTOR3 csmpos, csmvel, lmpos, csmnor, csmhvel;

	OBJHANDLE hbody=OurVessel->GetGravityRef();
	vh=1687.0;

	LightCompActy();
	if(ProgState == 0) {
		OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
		VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
		CSMVessel->GetRelativePos(hbody, csmpos);
		OurVessel->GetRelativePos(hbody, lmpos);
		radius=Mag(csmpos);
		rlm=Mag(lmpos);
		lmpos=lmpos*(radius/rlm);
		distance=Mag(lmpos-csmpos);
		CurrentRVel=distance;
		NextEventTime=simt+1.0;
		ProgFlag01=false;
		ProgFlag02=false;
		ProgState++;
		return;
	}
	if (ProgFlag01 && ProgFlag02) {
		AbortAscent(simt);
		return;
	}

	if(simt > NextEventTime) {
		NextEventTime=simt+1.0;
		if(ProgState >= 1) {
			OBJHANDLE hbody=OurVessel->GetGravityRef();
			double bradius=oapiGetSize(hbody);
			double bmass=oapiGetMass(hbody);
			mu=GRAVITY*bmass;

			OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
			VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
			CSMVessel->GetRelativePos(hbody, csmpos);
			CSMVessel->GetRelativeVel(hbody, csmvel);

			// aim for an orbit apo 25806 m (15 nm) lower than CSM apo (or semimajor)
			OrbitParams(csmpos, csmvel, csmperiod, csmapo, csmtta, csmper, csmttp);
//			a=(csmper+csmapo+2.0*bradius)/2.0;
//			dapo=(csmapo+bradius)-26806.0;
			dapo=(((csmapo+csmper)/2.0)+bradius)-26806.0;
			dper=bradius+18000.0;
			a=(dapo+dper)/2.0;
			dh=dapo-(bradius+87000.0);
			dv=dh/(((4.0*a*a)/mu)*1687.0);
//			DesiredLAN=sqrt(mu*((2.0/(csmapo+bradius-26806.0))-1.0/a));
//			sprintf(oapiDebugString(), "vh=%.3f", DesiredLAN);
			DesiredLAN=1687+dv;
//			sprintf(oapiDebugString(), "dh=%.1f dv=%.3f v=%.1f", dh, dv, DesiredLAN);

			OurVessel->GetRelativePos(hbody, lmpos);
			csmnor=CrossProduct(csmvel, csmpos);
			csmnor=Normalize(csmnor);
			CSMVessel->GetHorizonAirspeedVector(csmhvel);
			cbrg=atan2(csmhvel.x,csmhvel.z);
			if(csmhvel.x < 0.0) {
			//retrograde
				cbrg+=2*PI;
			} 
			DesiredAzimuth=cbrg*DEG;
			DesiredPlaneChange=lmpos*csmnor;
			radius=Mag(csmpos);
			rlm=Mag(lmpos);
			lmpos=lmpos*(radius/rlm);
			distance=Mag(lmpos-csmpos);
			vel=Mag(csmvel);
			if(distance < CurrentRVel) {
				time=distance/vel;
				if(ApolloNo < A14ASCENT) {
					BurnTime=simt+time+70.0;
				} else {
//					BurnTime=simt+time-50.0;  // TPI 10 min after insertion
//					BurnTime=simt+time-40.0;  // TPI 13 min
//					BurnTime=simt+time-35.0;  // TPI 53 min---
//					BurnTime=simt+time-30.0;  // TPI 57 min
					BurnTime=simt+time-50.0;  // historical TPI 46 min after insertion
//					BurnTime=simt+time-85.0;  //16 min, <2 min for higher orbits
				}
				BurnStartTime=BurnTime;
				ProgFlag01=true;
			}
			CurrentRVel=distance;
		}
	}


	if(ProgState == 1 && ProgFlag01) {
		SetVerbNounAndFlash(6,33);
		if(Realism == 0) ProgState++;
		return;
	}

	if(ProgState == 2) {
		DesiredDeltaVx=1687.0;
		DesiredDeltaVy=5.5714;
		SetVerbNounAndFlash(6,76);
		if(Realism == 0) ProgState++;
		return;
	}

	if(ProgState == 3 ) {
		SetVerbNounAndFlash(6,74);
		if(simt > BurnTime-35.0) {
			BlankAll();
			ProgState++;
		}
	}

	if(ProgState == 4 ) {
		if(simt > BurnTime-30.0) {
			UnBlankAll();
			SetVerbNounAndFlash(6,74);
			ProgState++;
		}
	}

	if(ProgState == 5) {
		if(simt > BurnTime-6.0) {
			SetVerbNounAndFlash(99,74);
			if(Realism == 0) {
				ProgState++;
				LEM *lem = (LEM *) OurVessel;
				lem->AbortStage();
			}
		} else {
			SetVerbNounAndFlash(6,74);
		}
	}


	if(ProgState == 6) {
		SetVerbNoun(6,74);
		if (simt >= BurnTime) {
			OurVessel->SetEngineLevel(ENGINE_HOVER, 1.0);
			ProgState=0;
			ProgFlag02=true;
			AbortAscent(simt);
		}
	}

}

void LEMcomputer::Prog12Pressed(int R1, int R2, int R3)
{
	switch (ProgState)
	{
	case 1:
		ProgState++;
		return;

	case 2:
		ProgState++;
		return;

	case 5:
		ProgState++;
		return;
	}
	LightOprErr();
}

void LEMcomputer::Prog70(double simt)
{
	SetStatus(simt,
               -1,
	            0,
	            0,
			   0);

	
	LightCompActy();
	if(DesiredLAN == 0.0) {
		// from NASA MSC 69-FM-46
		double t, vh;
		double k1=5644.2;
		double k2=0.09613;
		double k3=-0.0009345;
		double k4=0.0000006857;
		t=simt-DesiredPlaneChange;
		vh=(k1+k2*t+k3*t*t+k4*t*t*t)/3.25;
		if(vh < 1695.0) vh=1695.0;
//		sprintf(oapiDebugString(), "t=%.1f vh=%.3f", t, vh);
		DesiredLAN=vh;
		OurVessel->SetEngineLevel(ENGINE_HOVER, 1.0);
	}
	AbortAscent(simt);
}
void LEMcomputer::Prog71(double simt)
{
	const double GRAVITY=6.67259e-11;
//	char line[10];
	SetStatus(simt,
               -1,
	            0,
	            0,
			   0);


	LightCompActy();
	if(DesiredLAN == 0.0) {
		// from 69-FM-46
		double t, vh;
		double k1=5646.6;
		double k2=0.05157;
		double k3=-0.0007615;
		double k4=0.00000048214;
		t=simt-DesiredPlaneChange;
		vh=(k1+k2*t+k3*t*t+k4*t*t*t)/3.25;
		if(vh < 1695.0) vh=1695.0;
//		sprintf(oapiDebugString(), "t=%.1f vh=%.3f", t, vh);
		DesiredLAN=vh;
		/*
		// code for "anytime" abort...
		VECTOR3 csmpos, csmvel;
		double csmperiod, csmapo, csmper, csmtta, csmttp, phase, delta, a, pmin,
			tcatch, mu;
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		double bradius=oapiGetSize(hbody);
		double bmass=oapiGetMass(hbody);
		mu=GRAVITY*bmass;
		strncpy(line, OurVessel->GetName(), 10);
		line[6]=0;
		OBJHANDLE hcsm=oapiGetVesselByName(line);
		VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
		CSMVessel->GetRelativePos(hbody, csmpos);
		CSMVessel->GetRelativeVel(hbody, csmvel);
		OrbitParams(csmpos, csmvel, csmperiod, csmapo, csmtta, csmper, csmttp);
		Phase(phase, delta);
		if(phase < 0.0) {
			// need to catch up with CSM
			a=bradius+MINORBIT;
			pmin=2.0*PI*sqrt((a*a*a)/mu);
			delta=2.0*PI*((csmperiod-pmin)/(pmin*pmin));
			tcatch=phase/delta;
			if(tcatch < csmperiod*4.0) {
				DesiredLAN=sqrt(mu/(bradius+MINORBIT));
				// vertical velocity=0.0
			}
		} else {
			// need a long-period orbit to let CSM catch up
		}
		OurVessel->DeactivateNavmode(NAVMODE_KILLROT);
		OurVessel->SetEngineLevel(ENGINE_HOVER, 1.0);
		*/
	}
	AbortAscent(simt);
}

void LEMcomputer::AbortAscent(double simt)
{
	static VECTOR3 zero={0.0, 0.0, 0.0};
	const double GRAVITY=6.67259e-11;
	VECTOR3 csmpos, csmvel, csmnor, lmpos, lmvel, lmcsm, hvel, csmhvel, actatt, tgtatt,
		vec1, vec2, acc, lmnor, zerogl, ygl, zgl, lmdown, lmup, lmfor, vec, accvec;
	double crossrange, distance, delta, fuelflow, heading, Mass, totvel, altitude,
		tgo, velexh, veltbg, cbrg, phase, tau, A, B, C, D, D12, D21, E, L, crossvel,
		centrip, grav, acctot, vthrust, velo;
	LightCompActy();

//	velo=1687.0;
	velo=DesiredLAN;
	if(ProgState == 0) {
//		if(ABORTLOG) {
//			char fname[8];
//			sprintf(fname,"asclog.txt");
//			outstr=fopen(fname,"w");
//			fprintf(outstr, "Abort ascent beginning: ProgState=%d\n", ProgState);
//		}
		OurVessel->SetAttitudeRotLevel(zero);
		NextEventTime=simt+DELTAT;
		ProgState++;
		return;
	}
	if(ProgState == 3) {
		if(simt > BurnEndTime-0.05) {
			OurVessel->SetEngineLevel(ENGINE_HOVER, 0.0);
			OurVessel->SetAttitudeRotLevel(zero);
			ProgState++;
			NextEventTime=simt;
//			fclose(outstr);
			if (DesiredDeltaV) oapiSetTimeAcceleration(DesiredDeltaV);
//			sprintf(oapiDebugString(), "acc=%.1f", DesiredDeltaV);
			if(ApolloNo < A14ASCENT) {
				RunProgram(32);
			} else {
				RunProgram(34);
			}
			return;
		}
	}

	if(simt >= NextEventTime) {
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		double bradius=oapiGetSize(hbody);
		double bmass=oapiGetMass(hbody);
		NextEventTime=simt+DELTAT;

		OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
		VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
		CSMVessel->GetRelativePos(hbody, csmpos);
		CSMVessel->GetRelativeVel(hbody, csmvel);
		CSMVessel->GetHorizonAirspeedVector(csmhvel);
		cbrg=atan2(csmhvel.x,csmhvel.z);
		if(csmhvel.x < 0.0) {
		//retrograde
			cbrg+=2*PI;
		} 
		csmnor=CrossProduct(csmvel, csmpos);
		// this is the vector called Q
		csmnor=Normalize(csmnor);
		OurVessel->GetRelativePos(hbody, lmpos);
		OurVessel->GetRelativeVel(hbody, lmvel);
		lmdown=Normalize(-lmpos);
		lmup=Normalize(lmpos);
		lmfor=CrossProduct(lmup, csmnor);
		lmnor=CrossProduct(lmvel, lmpos);
		lmnor=Normalize(lmnor);
		OurVessel->GetHorizonAirspeedVector(hvel);
		altitude=OurVessel->GetAltitude();
		crossrange=lmpos*csmnor;
		crossvel=lmvel*csmnor;
		vec1=Normalize(csmpos);
		vec2=Normalize(lmpos);
		phase=acos(vec1*vec2);
		lmcsm=csmpos-lmpos;
		distance=Mag(lmcsm);
		lmcsm=csmvel-lmvel;
		delta=Mag(lmcsm);
		Mass=OurVessel->GetMass();
		vthrust=OurVessel->GetMaxThrust(ENGINE_HOVER);
		fuelflow=OurVessel->GetFuelRate();
//		fprintf(outstr,"ProgState=%d fuelflow=%.3f thrust=%.3f\n", ProgState, fuelflow, vthrust);
//		velexh=OurVessel->GetISP();
		velexh=2921.0;
		totvel=Mag(lmvel);
		veltbg=velo-totvel;
		CurrentVel=totvel;
		CurrentVelX=velo-sqrt(totvel*totvel-crossvel*crossvel-hvel.y*hvel.y);
		LandingAltitude=crossvel;
		tau=Mass/fuelflow;
		//estimate time-to-go
		tgo=tau*(1.0-exp(-(veltbg/velexh)));
		BurnEndTime=simt+tgo;
		//now compute the linear guidance coefficients
		L=log(1.0-tgo/tau);
		D12=tau+tgo/L;
		D21=tgo-D12;
		E=tgo/2.0-D21;
		B=(D21*(5.5714-hvel.y)-(18461.15-altitude-hvel.y*tgo))/(tgo*E);
		D=(D21*(-crossvel)-(-crossrange-crossvel*tgo))/(tgo*E);
		A=-D12*B-(5.5714-hvel.y)/L;
		C=-D12*D-(-crossvel)/L;
		centrip=(hvel.x*hvel.x+hvel.z*hvel.z)/(bradius+altitude);
		grav=(GRAVITY*bmass)/((bradius+altitude)*(bradius+altitude));
		acctot=vthrust/Mass;
		acc.y=(1.0/tau)*(A+B)-centrip+grav;
		acc.z=(1.0/tau)*(C+D);
// velocity to be gained in body up, right and forward direction
		DesiredDeltaVx=5.5714-hvel.y;
		DesiredDeltaVy=-crossvel;
		DesiredDeltaVz=velo-sqrt(totvel*totvel-crossvel*crossvel-hvel.y*hvel.y);

//		fprintf(outstr,"D12=%.3f D21=%.3f A=%.3f B=%.3f C=%.3f D=%.3f E=%.3f L=%.3f tau=%.3f\n",
//			D12, D21, A,B,C,D,E,L, tau);

		if(fabs(acc.z) > 1.0) {
			acc.z=1.0*(acc.z/fabs(acc.z));
		}
		acc.x=sqrt(acctot*acctot-acc.y*acc.y-acc.z*acc.z);
		TargetPitch=atan(acc.y/acc.x)-PI/2.0;
//		fprintf(outstr,"acc=%.3f %.3f %.3f cvel=%.3f\n",acc, crossvel);

		if(totvel < 50.0) {
			// use surface orientation for liftoff...
			actatt.x=OurVessel->GetPitch();
			actatt.z=-OurVessel->GetBank();
			oapiGetHeading(OurVessel->GetHandle(), &heading);
			actatt.y=cbrg-heading;
		} else {
			// once we have enough velocity, construct orbital reference orientation
			OurVessel->Local2Global(_V(0.0, 0.0, 0.0), zerogl);
			OurVessel->Local2Global(_V(0.0, 1.0, 0.0), vec1);
			OurVessel->Local2Global(_V(0.0, 0.0, 1.0), vec2);
			ygl=vec1-zerogl;
			zgl=vec2-zerogl;
			actatt.x=OurVessel->GetPitch();
			if(fabs(actatt.x) > 80.0*RAD) actatt.x=-PI/2.0-asin(lmdown*ygl);
			actatt.z=-asin(ygl*csmnor);
			actatt.y=asin(zgl*csmnor);
//			fprintf(outstr,"bank calc=%.3f %.3f cv=%.1f\n", 
//				actatt.z*DEG, OurVessel->GetBank()*DEG, crossvel);
		}

//		fprintf(outstr,"cross=%.1f dis=%.1f dv=%.1f tgo=%.1f f=%.1f m=%.1f p=%.1f\n",
//			crossrange, distance, delta, tgo, fuelflow, Mass, phase*DEG);
		// Vertical rise phase
		if(ProgState == 1) {
			if(hvel.y > 12.30) {
				tgtatt.x=TargetPitch;
				tgtatt.y=0.0;
				tgtatt.z=0.0;
				ProgState++;
			} else {
				tgtatt.x=0.0;
				tgtatt.y=0.0;
				tgtatt.z=0.0;
			}
		}

		if(ProgState == 2) {
			tgtatt.x=TargetPitch;
			tgtatt.y=0.0;
			tgtatt.z=-asin(acc.z/acctot);
			if(tgo < 15.0) {
				// Turn off time acceleration if greater 1
				if (oapiGetTimeAcceleration() > 1.0) {
					DesiredDeltaV=oapiGetTimeAcceleration();
					oapiSetTimeAcceleration(1.0);
				} else {
					DesiredDeltaV=0;
				}
			}
			if(tgo < 2.0) {
				ProgState++;
			}

		}
//		fprintf(outstr, "tgt=%.3f %.3f %.3f act=%.3f %.3f %.3f \n",
//			tgtatt*DEG, actatt*DEG);

		if (ProgState < 4) {
			// for large pitch changes, make sure we pitch keeping thrust 
			// in the UP direction...
			if((actatt.x-tgtatt.x) > 20.0*RAD ) tgtatt.x=actatt.x-20.0*RAD;
			if(totvel < 100.0) {
				ComAttitude(actatt, tgtatt, false);
			} else {
				vec=lmfor*acc.x+lmup*acc.y+csmnor*acc.z;
				accvec=Normalize(vec);
				OrientAxis(accvec, 1, 0);
//				fprintf(outstr,"vec=%.3f %.3f %.3f\n", accvec);
			}
		}
	}

	if (ProgState == 1) SetVerbNoun(16, 77);
	if (ProgState == 4) {
		SetVerbNounAndFlash(6, 85);
		ProgState++;
	}

}

void LEMcomputer::Prog30(double simt)
{
//	VECTOR3 posm, velm, posv, velv;
//	double emass, mmass;
//	OBJHANDLE hmoon=oapiGetGbodyByName("moon");
//	OBJHANDLE hearth=oapiGetGbodyByName("earth");
//	mmass=oapiGetMass(hmoon);
//	emass=oapiGetMass(hearth);
//	oapiGetRelativePos(hmoon, hearth, posm);
//	oapiGetRelativeVel(hmoon, hearth, velm);
//	OurVessel->GetRelativePos(hearth, posv);
//	OurVessel->GetRelativeVel(hearth, velv);
}
	/*
	const double GRAVITY=6.67259e-11;
	VECTOR3 pos, vel, b, norm, spos, svel, apos, avel, v1, v2, dv1, forward;
	double period, apo, tta, per, ttp, vmass, mu, vlat, vlon, vrad, pday, offplane,
		time, dlon, blat, blon, dlat, aa, cc, sbdis, tpass, velm, btime,
		atime, dt, visp, fmass, thrust, rb, aheight;
	int npass;
	bool intersect;
	LightCompActy();
	if(ProgState == 0) {
		ProgState++;
		NextEventTime=simt+DELTAT;
		return;
	}
	if(simt > NextEventTime) {
		aheight=110000.0;
		NextEventTime=simt+0.1;
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		double bradius=oapiGetSize(hbody);
		double bmass=oapiGetMass(hbody);
		mu=GRAVITY*bmass;
		OurVessel->GetRelativePos(hbody, pos);
		OurVessel->GetRelativeVel(hbody, vel);
		norm=Normalize(CrossProduct(vel, pos));
		pday=oapiGetPlanetPeriod(hbody);
		vmass=OurVessel->GetMass();
		OrbitParams(pos, vel, period, apo, tta, per, ttp);
//		ve=Normalize((pos*(Mag(vel)*Mag(vel)-(mu/Mag(pos)))-vel*(pos*vel))/mu);
		OurVessel->GetEquPos(vlon, vlat, vrad);
//		sprintf(oapiDebugString(), "lat=%.3f lon=%.3f", vlat*DEG, vlon*DEG);
		blat=LandingLatitude*RAD;
		blon=LandingLongitude*RAD;
		dlat=vlat-blat;
		dlon=vlon-blon;
		aa = pow(sin(dlat/2), 2) + cos(blat) * cos (vlat) * pow(sin(dlon/2), 2);
		cc = 2 * atan2(sqrt(aa), sqrt(1 - aa));
		sbdis=bradius*cc;

		tpass=(cc/(2.0*PI))*period;

		npass=0;
		time=tpass;
		dlon=(time/pday)*PI;
		vlat=LandingLatitude*RAD;
		vlon=LandingLongitude*RAD+dlon;
		if(vlon < -PI) dlon=dlon+2.0*PI;
		if(vlon > PI) dlon=dlon-2.0*PI;
		vrad=bradius;
		EquToRel(vlat, vlon, vrad, b);
		offplane=b*norm;
		thrust=OurVessel->GetMaxThrust(ENGINE_HOVER);
		visp=OurVessel->GetISP();

//		PredictPosVelVectors(pos, vel, mu, period*PI, spos, svel, velm);
//		PredictPosVelVectors(spos, svel, mu, -period*PI, apos, avel, velm);
//		sprintf(oapiDebugString(), "p1=%.1f %.1f %.1f p2=%.1f %.1f %.1f", pos, apos);
//		return;


		intersect=false;
		if(apo >= aheight) {
			if(per <= aheight) intersect=true;
		}
//		sprintf(oapiDebugString(), "day=%.8f off=%.1f", 
//			pday, offplane);
		if(tta < ttp) {
			if(apo < aheight) {
				if(tta < period/4.0) tta=period/4.0;
			// calc a burn to get us to 110 km - starting 180 sec from now
				// predict burn position 180 secs from now
				btime=180.0;
				PredictPosVelVectors(pos, vel, mu, btime, spos, svel, velm);
				// predict apocenter position if no burn were done
				atime=tta;
				PredictPosVelVectors(pos, vel, mu, atime, apos, avel, velm);
				// calc position of apo if it were 110 km altitude
				apos=Normalize(apos)*(bradius+aheight);
				// calculate what our velocity needs to be to get there
				dt=atime-180.0;
				Lambert(spos, apos, dt, mu, v1, v2);
				// calculate difference from current velocity
				dv1=v1-svel;
				DesiredDeltaVx=dv1.x;
				DesiredDeltaVy=dv1.y;
				DesiredDeltaVz=dv1.z;
				// calculate fuel needed for dV
				fmass=vmass*(1.0-exp(-(Mag(dv1)/visp)));
				BurnStartTime=simt+180.0-(fmass/(thrust/visp));
				BurnEndTime=simt+180.0;
				BurnTime=BurnEndTime+dt;
//				sprintf(oapiDebugString(), "dv=%.3f %.3f %.3f st=%.1f end=%.1f", dv1,
//					BurnStartTime, BurnEndTime);
				DesiredLAN=30;
				// ask P40 to do the burn
				RunProgram(40);
			} else {
				// when we are at 110 lm alt, circularize
				btime=BurnTime-simt;
				PredictPosVelVectors(pos, vel, mu, btime, spos, svel, velm);
				rb=Mag(spos);
				norm=Normalize(CrossProduct(svel, spos));
				forward=Normalize(CrossProduct(spos, norm));
				v1=forward*sqrt(mu/rb);  // velocity for circular orbit at radius rb
				dv1=v1-svel;
				DesiredDeltaVx=dv1.x;
				DesiredDeltaVy=dv1.y;
				DesiredDeltaVz=dv1.z;
				fmass=vmass*(1.0-exp(-(Mag(dv1)/visp)));
				BurnStartTime=simt+btime-(fmass/(thrust/visp));
				BurnEndTime=simt+btime;
//				sprintf(oapiDebugString(), "dv=%.3f %.3f %.3f st=%.1f end=%.1f rad=%.1f v=%.1f", dv1,
//					BurnStartTime, BurnEndTime, rb, Mag(v1));
				DesiredLAN=0;
				RunProgram(40);	
			}
		} else {
			// burn at pericenter to raise apo to >= 110 km
		}
	}  // end of 2-sec guidance 


/*
void LEMcomputer::EquToRel(double vlat, double vlon, double vrad, VECTOR3 &pos)
{
		VECTOR3 a;
		double obliq, theta, rot, pday, date;
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		a.x=cos(vlat)*cos(vlon)*vrad;
		a.z=cos(vlat)*sin(vlon)*vrad;
		a.y=sin(vlat)*vrad;
		pday=oapiGetPlanetPeriod(hbody);
		obliq=oapiGetPlanetObliquity(hbody);
		theta=oapiGetPlanetTheta(hbody);
		rot=oapiGetPlanetCurrentRotation(hbody);
		date=oapiGetSimMJD();
//		sprintf(oapiDebugString(), "per=%.5f ob=%.15f rot=%.12f date=%.8f theta=%.15f",
//			pday, obliq, rot, date, theta);
		pos.x=a.x*(cos(theta)*cos(rot)-sin(theta)*cos(obliq)*sin(rot))-
			a.y*sin(theta)*sin(obliq)-
			a.z*(cos(theta)*sin(rot)+sin(theta)*cos(obliq)*cos(rot));
		pos.y=a.x*(-sin(obliq)*sin(rot))+
			a.y*cos(obliq)-
			a.z*sin(obliq)*cos(rot);
		pos.z=a.x*(sin(theta)*cos(rot)+cos(theta)*cos(obliq)*sin(rot))+
			a.y*cos(theta)*sin(obliq)+
			a.z*(-sin(theta)*sin(rot)+cos(theta)*cos(obliq)*cos(rot));

}
*/
void LEMcomputer::Phase(double &phase, double &delta)
{
	const double GRAVITY=6.67259e-11;
	VECTOR3 csmpos, csmvel, lmpos, lmvel;
	double vdotr, lmperiod, lmapo, lmtta, lmper, lmttp, csmperiod, csmapo,
		csmtta, csmper, csmttp;

	OBJHANDLE hbody=OurVessel->GetGravityRef();

	OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
	VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
	CSMVessel->GetRelativePos(hbody, csmpos);
	CSMVessel->GetRelativeVel(hbody, csmvel);
	OrbitParams(csmpos, csmvel, csmperiod, csmapo, csmtta, csmper, csmttp);
	OurVessel->GetRelativePos(hbody, lmpos);
	OurVessel->GetRelativeVel(hbody, lmvel);
	OrbitParams(lmpos, lmvel, lmperiod, lmapo, lmtta, lmper, lmttp);
	phase=acos(Normalize(lmpos)*Normalize(csmpos));
	vdotr=Normalize(csmvel)*Normalize(lmpos);
	if(vdotr < 0.0) phase=-phase;  //csm is ahead of the LM
	delta=2.0*PI*((csmperiod-lmperiod)/(lmperiod*lmperiod));
}

VESSEL * LEMcomputer::GetCSM()
{
	OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
	VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
	return CSMVessel;
}

void LEMcomputer::Radar(double &range, double &rate)
{
	VECTOR3 csmpos, csmvel, lmpos, lmvel;
	OBJHANDLE hbody=OurVessel->GetGravityRef();

	VESSEL *CSMVessel=GetCSM();
	CSMVessel->GetRelativePos(hbody, csmpos);
	CSMVessel->GetRelativeVel(hbody, csmvel);
	OurVessel->GetRelativePos(hbody, lmpos);
	OurVessel->GetRelativeVel(hbody, lmvel);
	if(range > 740800.0) {
		range=-1.0;
		rate=0.0;
	} else {
		range=Mag(csmpos-lmpos);
		rate=(lmvel-csmvel)*Normalize(csmpos-lmpos);
	}
}

void LEMcomputer::GetIMUOrientation(int type, double arg, VECTOR3 &x, VECTOR3 &y, VECTOR3 &z)
{
	// this code is actually calculating the IMU orientation...
	OBJHANDLE earth, moon;
	VECTOR3 rpos, rvel, east, north, csmpos, csmvel, norm;
	double vrad, vlat, vlon, pday;
    char line[10];
	switch (type) {
	// Launch pad alignment
	case 0:
		earth=oapiGetGbodyByName("earth");
		OurVessel->GetRelativePos(earth, rpos);
		OurVessel->GetRelativeVel(earth, rvel);
		y=Normalize(-rpos);
		east=Normalize(CrossProduct(y, _V(0.0, 1.0, 0.0)));
		north=CrossProduct(y, east);
		//arg needs to be set to the launch azimuth
		x=Normalize(east*sin(arg)+north*cos(arg));
		z=CrossProduct(x, y);
		break;
	//PTC reference alignment
	case 1:
		earth=oapiGetGbodyByName("earth");
		moon=oapiGetGbodyByName("moon");
        break;
	//Circularization through lunar landing alignment
	case 3:
		// for this case, arg is seconds to landing time...
		moon=oapiGetGbodyByName("moon");
		pday=oapiGetPlanetPeriod(moon);
		vrad=oapiGetSize(moon);
		vlat=LandingLatitude*RAD;
		vlon=LandingLongitude*RAD+pday*arg;
		EquToRel(vlat, vlon, vrad, rpos);
		x=Normalize(rpos);
		strncpy(line, OurVessel->GetName(), 10);
		line[6]=0;
		OBJHANDLE hcsm=oapiGetVesselByName(line);
		VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
		CSMVessel->GetRelativePos(moon, csmpos);
		CSMVessel->GetRelativeVel(moon, csmvel);
		// normal to csm orbit plane
		norm=Normalize(CrossProduct(csmpos, csmvel));
		z=Normalize(CrossProduct(norm, x));
		y=Normalize(CrossProduct(z, x));
		break;
		
	}
}


//
// CSI program calculates the apoapsis burn to circularize the LM orbit
//

void LEMcomputer::Prog32(double simt)
{
	const double GRAVITY=6.67259e-11;
	VECTOR3 lmpos, lmvel, apos, avel, vec, csmpos;
	double mu, period, ttp, tta, rapo, vapo, velm, dv, t12, vmass, apo, per,  
		 ythrust;
	int i, nt;

	LightCompActy();
	if(ProgState == 0) {
			ProgState++;
			OBJHANDLE hbody=OurVessel->GetGravityRef();
			double bradius=oapiGetSize(hbody);
			double bmass=oapiGetMass(hbody);
			mu=GRAVITY*bmass;
			OurVessel->GetRelativePos(hbody, lmpos);
			OurVessel->GetRelativeVel(hbody, lmvel);
			vmass=OurVessel->GetMass();
			OrbitParams(lmpos, lmvel, period, apo, tta, per, ttp);
			PredictPosVelVectors(lmpos, lmvel, mu, tta, apos, avel, velm);
			rapo=Mag(apos);
			vapo=sqrt(mu/rapo);
			dv=vapo-velm;
			avel=Normalize(avel)*dv;
			ythrust=0.0;
			THRUSTER_HANDLE th;
			nt=OurVessel->GetGroupThrusterCount(THGROUP_ATT_UP);
			for (i=0; i<nt; i++) 
			{
				th=OurVessel->GetGroupThruster(THGROUP_ATT_UP, i);
				ythrust=ythrust+OurVessel->GetThrusterMax(th);
			}
			t12=((vmass/ythrust)*dv)/2.0;
			BurnTime=simt+tta-t12;
			BurnStartTime=BurnTime;
			BurnEndTime=BurnStartTime+2.0*t12;
			DesiredDeltaVx=avel.x;
			DesiredDeltaVy=avel.y;
			DesiredDeltaVz=avel.z;
			NextEventTime=simt+DELTAT;
			CutOffVel=simt+100.0;
			SetVerbNoun(16, 39);
	}

	if(simt > NextEventTime) {
		if(simt > CutOffVel) {
			ProgState=0;
			return;
		}
		if(ProgState == 1) {
			OBJHANDLE hbody=OurVessel->GetGravityRef();

			OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
			VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
			CSMVessel->GetRelativePos(hbody, csmpos);
			OurVessel->GetRelativePos(hbody, lmpos);
			vec=Normalize(csmpos-lmpos);
			OrientAxis(vec, 2, 0);
//			sprintf(oapiDebugString(), "CSI time=%.1f", BurnStartTime-simt);
			if(BurnStartTime-simt < 120.0)	{
				DesiredLAN=33;
				DesiredPlaneChange=1.0;
				RunProgram(41);
				return;
			}
		}
	}
}

//
// CDH program to adjust orbit to constant delta height with CSM
//

void LEMcomputer::Prog33(double simt)
{
	const double GRAVITY=6.67259e-11;
	VECTOR3 csmpos, csmvel, lmpos, lmvel, vec, avel, apos;
	VECTOR3 lmposcdh, lmvelcdh;
	double csmperiod, csmapo, csmtta, csmper, csmttp, lmperiod, lmapo, lmtta, lmper, 
		phase, lmttp, vdotr, delta, adh, theta, cdhtim, tpitime, a, e, h, vnew, 
		mu, drad, velm, t12, vmass, newapo, newper, samer, ddv, ythrust, lmvelmagcdh;
	int i, nt;
	LightCompActy();
	if(ProgState == 0) {
		ProgState++;
		ProgFlag01=false;
		NextEventTime=simt+DELTAT;
		return;
	} 
	if(simt > NextEventTime) {
		NextEventTime=simt+DELTAT;
		if(ProgState == 1) {
			OBJHANDLE hbody=OurVessel->GetGravityRef();
			double bradius=oapiGetSize(hbody);
			double bmass=oapiGetMass(hbody);
			mu=GRAVITY*bmass;

			OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
			VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
			CSMVessel->GetRelativePos(hbody, csmpos);
			CSMVessel->GetRelativeVel(hbody, csmvel);
			OrbitParams(csmpos, csmvel, csmperiod, csmapo, csmtta, csmper, csmttp);
			//skip cdh if csm orbit is already close to circular
			if(csmapo-csmper < 1000.0) {
				RunProgram(34);
				return;
			}
			OurVessel->GetRelativePos(hbody, lmpos);
			OurVessel->GetRelativeVel(hbody, lmvel);
			OrbitParams(lmpos, lmvel, lmperiod, lmapo, lmtta, lmper, lmttp);
			phase=acos(Normalize(lmpos)*Normalize(csmpos));
			vdotr=Normalize(csmvel)*Normalize(lmpos);
			if(vdotr < 0.0) phase=-phase;  //csm is ahead of the LM
			delta=2.0*PI*((csmperiod-lmperiod)/csmperiod);
			adh=(csmapo-lmapo);
			theta=(adh/tan(26.0*RAD))*(-1.0/Mag(lmpos));
			tpitime=csmperiod*((theta-phase)/delta);
			// calculate time until LM is at CSM's apoapsis or periapsis position
			if(csmtta < csmttp) {
				cdhtim=csmtta-((phase+delta*(csmtta/csmperiod))/(2.0*PI))*lmperiod;
				PredictPosVelVectors(lmpos, lmvel, mu, cdhtim, lmposcdh, lmvelcdh, lmvelmagcdh);				
				newapo = Mag(lmposcdh) - bradius;
				newper = newapo - (csmapo - csmper);
				samer = newapo + bradius;
			} else {
				cdhtim=csmttp-((phase+delta*(csmttp/csmperiod))/(2.0*PI))*lmperiod;
				PredictPosVelVectors(lmpos, lmvel, mu, cdhtim, lmposcdh, lmvelcdh, lmvelmagcdh);				
				newper = Mag(lmposcdh) - bradius;
				newapo = newper + (csmapo - csmper);
				samer = newper + bradius;
			}
			// if cdh time is after tpi time, skip cdh
			if(tpitime > 0 && cdhtim > tpitime-150.0) {
				RunProgram(34);
				return;
			}
			vmass=OurVessel->GetMass();
			PredictPosVelVectors(lmpos, lmvel, mu, cdhtim, apos, avel, velm);
			// calc dv for apside change
			drad=csmapo-csmper;
			a=(newapo+newper+(2.0*bradius))/2.0;
			e=((newapo+bradius)/a)-1.0;
			h=sqrt(mu*a*(1-e*e));
			vnew=h/samer;
			ddv=fabs(Mag(avel)-vnew);
			avel=Normalize(avel);
			if(csmtta < csmttp) avel=-avel;

			ythrust=0.0;
			THRUSTER_HANDLE th;
			nt=OurVessel->GetGroupThrusterCount(THGROUP_ATT_UP);
			for (i=0; i<nt; i++) 
			{
				th=OurVessel->GetGroupThruster(THGROUP_ATT_UP, i);
				ythrust=ythrust+OurVessel->GetThrusterMax(th);
			}

			t12=((vmass/TRTHRUST)*fabs(ddv))/2.0;
			BurnTime=t12*2.0;
			if(BurnTime < 1.0) {
				DesiredPlaneChange=0.1;
				BurnTime=10.0*BurnTime;
			} else {
				DesiredPlaneChange=1.0;
			}
			BurnStartTime=simt+cdhtim-BurnTime/2.0;
			BurnEndTime=BurnStartTime+BurnTime;
			SetVerbNoun(16, 39);

			DesiredDeltaVx=avel.x;
			DesiredDeltaVy=avel.y;
			DesiredDeltaVz=avel.z;

			CutOffVel=simt+100.0;
			ProgState++;
		}
		if(ProgState == 2) {
			OBJHANDLE hbody=OurVessel->GetGravityRef();

			OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
			VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
			CSMVessel->GetRelativePos(hbody, csmpos);
			OurVessel->GetRelativePos(hbody, lmpos);
			vec=Normalize(csmpos-lmpos);
			OrientAxis(vec, 2, 0);
			// ordinarily we would call P41 here
			if(simt > BurnStartTime-120.0) {
				ProgState=0;
				DesiredLAN=34;
				RunProgram(41);
				return;
			}
		}
	}
}

//
// TPI program to calculate the transfer phase for rendezvous
//

void LEMcomputer::Prog34(double simt)
{
	const double GRAVITY=6.67259e-11;
	VECTOR3 csmpos, csmvel, lmpos, lmvel, renpos, renvel, stpos, stvel, v1, v2, dv1,
		rpos, rvel, acc, s, vec, cstpos, cstvel, uu, ul, up;
	double mu, time, angle, dt, delta, t, q, ea, ythrust,
		velm, dis,  tpitime, to, ao, elev, dadt, dd, vmass,
		visp, thrust, fmass, csmperiod, csmapo, csmtta, csmper, csmttp, lmperiod, lmapo, 
		lmtta, lmper, lmttp, phase, vdotr, adh, theta, x0, x1, x2, x3,
		f0, f1, f2, f3, c, r, ax, bx, cx, fax, fbx, fcx, dr;
	int k, nt;


	LightCompActy();
	if(ProgState == 0) {
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		double bradius=oapiGetSize(hbody);
		double bmass=oapiGetMass(hbody);

		OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
		VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
		CSMVessel->GetRelativePos(hbody, csmpos);
		CSMVessel->GetRelativeVel(hbody, csmvel);
		OrbitParams(csmpos, csmvel, csmperiod, csmapo, csmtta, csmper, csmttp);
		OurVessel->GetRelativePos(hbody, lmpos);
		OurVessel->GetRelativeVel(hbody, lmvel);
		OrbitParams(lmpos, lmvel, lmperiod, lmapo, lmtta, lmper, lmttp);
		Phase(phase, delta);
		adh = ((csmapo-lmapo) + (csmper - lmper)) / 2.0;
		theta=(adh/tan(26.0*RAD))*(-1.0/Mag(lmpos));
		tpitime=(theta-phase)/delta;
		mu=GRAVITY*bmass;

//		char fname[8];
//		sprintf(fname,"P34log.txt");
//		outstr=fopen(fname,"w");
//		fprintf(outstr, "P34: ProgState=%d No=%d %.3f\n", ProgState, ApolloNo, phase*DEG);
		if (ApolloNo < A14ASCENT) {
			if(Mag(csmpos-lmpos) > 740800.0) {
				// out of radar range...
				AbortWithError(0526);
				return;
			}
			elev=26.0*RAD;  //target elevation
			if (adh < 0) elev = -elev;
			time=tpitime;
			// do this the way A11 and A12 did, looking for 26 degrees elevation...
			for (k=0; k<1000; k++) {
				// predict where the CSM will be at TPI time
				PredictPosVelVectors(csmpos, csmvel, mu, time, v1, renvel, velm);
				PredictPosVelVectors(lmpos, lmvel, mu, time, v2, stvel, velm);
//					fprintf(outstr,"time=%.1f csm=%.1f %.1f %.1f lm=%.1f %.1f %.1f \n",
//						time, v1, v2);
				delta=Mag(v1)-Mag(v2);  //vertical distance
				dd=Mag(v1-v2);  // slant distance
				dis=sqrt(dd*dd-delta*delta);  //horizontal distance
				angle=atan(delta/dis);
//					fprintf(outstr, "it=%d time=%.1f angle=%.3f sl=%.1f del=%.1f hor=%.1f\n", 
				if(fabs(angle-elev) < 0.000001*RAD) {
//						fprintf(outstr, "Converged\n");
					break;
				}
				if(k>0) {
					dadt=(angle-ao)/(time-to);
					to = time;
					ao = angle;
					time = time + max(-60.0, min(60.0, (elev - angle) / dadt));
				} else {
					ao=angle;
					to=time;
					if(angle < elev) {
						time=time+60.0;
					} else {
						time=time-60.0;
					}
				}
			}
			BurnTime=time+simt;
		} else {
			// what we're doing here is to iteratively solve for the lowest dV
			// transfer time to get to the CSM.  First we bracket the endpoints 
			// of the search, then use golden section to converge on the tpitime
			tpitime=(-phase)/delta;
			ax=180.0;
			cx = -phase/delta + 1000.0;
//				fprintf(outstr, "phase=%.3f del=%.5f tpi=%.1f mx=%.1f\n",
//					phase*DEG, delta*DEG, tpitime, cx);
			// use golden section to find minimum delta V for transfer...
			r=0.61803399;
			c=1.0-r;
			// transfer time fixed at 45 minutes...
			dt=45.0*60.0;
			time=ax;
			PredictPosVelVectors(csmpos, csmvel, mu, time+dt, renpos, renvel, velm);
			PredictPosVelVectors(lmpos, lmvel, mu, time, stpos, stvel, velm);
			Lambert(stpos, renpos, dt, mu, v1, v2);
			fax=Mag(v1-stvel)+Mag(v2-renvel);
//				fprintf(outstr, "ax=%.3f fax=%.3f \n", ax, fax);
			time=cx;
			PredictPosVelVectors(csmpos, csmvel, mu, time+dt, renpos, renvel, velm);
			PredictPosVelVectors(lmpos, lmvel, mu, time, stpos, stvel, velm);
			Lambert(stpos, renpos, dt, mu, v1, v2);
			fcx=Mag(v1-stvel)+Mag(v2-renvel);
//				fprintf(outstr, "cx=%.3f fcx=%.3f \n", cx, fcx);
			if(tpitime > ax && tpitime < cx) {
				bx=tpitime;
			} else {
				bx=(ax+cx)/2.0;
			}
			time=bx;
			PredictPosVelVectors(csmpos, csmvel, mu, time+dt, renpos, renvel, velm);
			PredictPosVelVectors(lmpos, lmvel, mu, time, stpos, stvel, velm);
			Lambert(stpos, renpos, dt, mu, v1, v2);
			fbx=Mag(v1-stvel)+Mag(v2-renvel);
//				fprintf(outstr, "bx=%.3f fbx=%.3f \n", bx, fbx);
			// test for bracketing...
			if(fbx > fax || fbx > fcx) {
				// In this case no optimization is possible, so we try find a not too bad 
				// TPI time with brute force since there are no alternatives (except of giving up...)
				double maxVel = 0;
				for (k = 180; k < lmperiod; k += 10) {
					PredictPosVelVectors(csmpos, csmvel, mu, k+dt, renpos, renvel, velm);
					PredictPosVelVectors(lmpos, lmvel, mu, k, stpos, stvel, velm);
					Lambert(stpos, renpos, dt, mu, v1, v2);
					fcx=Mag(v1-stvel)+Mag(v2-renvel);

					if (k == 180 || Mag(v2-renvel) < maxVel) {
						maxVel = Mag(v2-renvel);
						tpitime = k;
					}
//					fprintf(outstr, "t=%.3f v=%.3f vacc=%.3f vbreak=%.3f\n", (double) k, fcx, Mag(v1-stvel), Mag(v2-renvel));
				}
//				fprintf(outstr, "tpitime=%.3f maxVel=%.3f \n", tpitime, maxVel);
			} else {
				x0=ax;
				x3=cx;
				if(fabs(cx-bx) > fabs(bx-ax)) {
					x1=bx;
					x2=bx+c*(cx-bx);
				} else {
					x2=bx;
					x1=bx-c*(bx-ax);
				}
				time=x1;
				PredictPosVelVectors(csmpos, csmvel, mu, time+dt, renpos, renvel, velm);
				PredictPosVelVectors(lmpos, lmvel, mu, time, stpos, stvel, velm);
				Lambert(stpos, renpos, dt, mu, v1, v2);
				f1=Mag(v1-stvel)+Mag(v2-renvel);
	//				fprintf(outstr, "x1=%.3f f1=%.3f \n", x1, f1);
				time=x2;
				PredictPosVelVectors(csmpos, csmvel, mu, time+dt, renpos, renvel, velm);
				PredictPosVelVectors(lmpos, lmvel, mu, time, stpos, stvel, velm);
				Lambert(stpos, renpos, dt, mu, v1, v2);
				f2=Mag(v1-stvel)+Mag(v2-renvel);
	//				fprintf(outstr, "x2=%.3f f2=%.3f \n", x2, f2);
				for (k=0; k<30; k++) {
	//					fprintf(outstr, "k=%d x0=%.3f x1=%.3f x2=%.3f x3=%.3f \n",
	//						k, x0, x1, x2, x3);
					if(fabs(x3-x0) > 0.0001*(fabs(x1)+fabs(x2))) {
						if(f2 < f1) {
							x0=x1;
							x1=x2;
							x2=r*x1+c*x3;
							f0=f1;
							f1=f2;
							time=x2;
							PredictPosVelVectors(csmpos, csmvel, mu, time+dt, renpos, renvel, velm);
							PredictPosVelVectors(lmpos, lmvel, mu, time, stpos, stvel, velm);
							Lambert(stpos, renpos, dt, mu, v1, v2);
							f2=Mag(v1-stvel)+Mag(v2-renvel);
	//							fprintf(outstr, "x2=%.3f f2=%.3f \n", x2, f2);
						} else {
							x3=x2;
							x2=x1;
							x1=r*x2+c*x0;
							f3=f2;
							f2=f1;
							time=x1;
							PredictPosVelVectors(csmpos, csmvel, mu, time+dt, renpos, renvel, velm);
							PredictPosVelVectors(lmpos, lmvel, mu, time, stpos, stvel, velm);
							Lambert(stpos, renpos, dt, mu, v1, v2);
							f1=Mag(v1-stvel)+Mag(v2-renvel);
	//							fprintf(outstr, "x1=%.3f f1=%.3f \n", x1, f1);
						}
					} else {
						break;
					}
				}
				if(f1 < f2) {
					tpitime=x1;
				} else {
					tpitime=x2;
				}
			}
			PredictPosVelVectors(csmpos, csmvel, mu, tpitime, renpos, renvel, velm);
			phase=acos(Normalize(stpos)*Normalize(renpos));
			vdotr=Normalize(renvel)*Normalize(stpos);
			if(vdotr < 0.0) phase=-phase;  //csm is ahead of the LM
			dr=Mag(renpos)-Mag(stpos);
			x1=Mag(stpos-renpos);
			elev=asin(dr/x1);
//				fprintf(outstr, "phase=%.3f dr=%.1f elev=%.3f\n", phase*DEG, dr, elev*DEG);
			BurnTime=tpitime+simt;
		}
//		fclose(outstr);
		BurnStartTime=BurnTime;
		SetVerbNoun(16, 39);
		ProgState++;
	}

	if(ProgState == 2) {
//			char fname[8];
//			sprintf(fname,"P34log.txt");
//			outstr=fopen(fname,"w");
//			fprintf(outstr, "P34: ProgState=%d\n", ProgState);
		ProgState++;
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		double bradius=oapiGetSize(hbody);
		double bmass=oapiGetMass(hbody);
		mu=GRAVITY*bmass;
		if(ApolloNo < A14ASCENT) {
			ythrust=0.0;
			THRUSTER_HANDLE th;
			nt=OurVessel->GetGroupThrusterCount(THGROUP_ATT_UP);
			for (k=0; k<nt; k++) 
			{
				th=OurVessel->GetGroupThruster(THGROUP_ATT_UP, k);
				ythrust=ythrust+OurVessel->GetThrusterMax(th);
			}
			thrust=ythrust;
		} else {
			thrust=OurVessel->GetMaxThrust(ENGINE_HOVER);
		}
		visp=OurVessel->GetISP();

		OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
		VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
		CSMVessel->GetRelativePos(hbody, csmpos);
		CSMVessel->GetRelativeVel(hbody, csmvel);
		tpitime=BurnTime-simt;
		dt=45.0*60.0;
		time=tpitime+dt;
		// save rendezvous time
		DeltaPitchRate=simt+time;
		// predict where the CSM will be at rendezvous time
		PredictPosVelVectors(csmpos, csmvel, mu, time, renpos, renvel, velm);
//			fprintf(outstr, "Rendezvous time: %.1f simt=%.1f pos=%.1f %.1f %.1f\n",
//				time, simt+time, renpos);
		LandingLatitude=renpos.x;
		LandingLongitude=renpos.y;
		LandingAltitude=renpos.z;
		OurVessel->GetRelativePos(hbody, lmpos);
		OurVessel->GetRelativeVel(hbody, lmvel);
		vmass=OurVessel->GetMass();
		time=tpitime;
//			fprintf(outstr, "Current pos=%.1f %.1f %.1f vel=%.1f %.1f %.1f\n", lmpos, lmvel);
		// predict where the lm will be at TPI time
		PredictPosVelVectors(lmpos, lmvel, mu, time, stpos, stvel, velm);
		dis=Mag(stpos-lmpos);

		// compute elevation angle at TPI
		//predict csm pos and vel at TPI time
		PredictPosVelVectors(csmpos, csmvel, mu, time, cstpos, cstvel, velm);
		ul=Normalize(cstpos-stpos);
		uu=Normalize(CrossProduct(stpos, stvel));
		up=Normalize(CrossProduct(uu, stpos));
		q=Normalize(stpos)*ul;
		q=q/fabs(q);
		ea=acos((ul*up)*q);
//			sprintf(oapiDebugString(), "elev=%.3f q=%.1f", ea*DEG, q);
		// end of elevation calc
//			fprintf(outstr, "LM TPI  pos=%.1f %.1f %.1f vel=%.1f %.1f %.1f dis=%.1f time=%.1f\n", 
//				stpos, stvel, dis, simt+tpitime);
		DesiredApogee=stpos.x;
		DesiredPerigee=stpos.y;
		DesiredAzimuth=stpos.z;

		// compute starting and ending velocities for the transfer
		Lambert(stpos, renpos, dt, mu, v1, v2);

//			fprintf(outstr," Lambert r1=%.1f %.1f %.1f r2=%.1f %.1f %.1f \n", stpos, renpos);
//			fprintf(outstr," Lambert v1=%.1f %.1f %.1f v2=%.1f %.1f %.1f \n", v1, v2);

		dv1=v1-stvel;
		DesiredDeltaVx=dv1.x;
		DesiredDeltaVy=dv1.y;
		DesiredDeltaVz=dv1.z;
		// advance ignition time the length of the burn
		t=(vmass/thrust)*Mag(dv1);
		acc=dv1/t;
		s=acc*((t*t)/2.0);

//			fprintf(outstr,"Burn displacement=%.3f %.3f %.3f Duration=%.3f\n",s, t);

		Lambert(stpos+s, renpos, dt, mu, v1, v2);
//			fprintf(outstr," Lambert r1=%.1f %.1f %.1f r2=%.1f %.1f %.1f \n", stpos+s, renpos);
//			fprintf(outstr," Lambert v1=%.1f %.1f %.1f v2=%.1f %.1f %.1f \n", v1, v2);
		OrbitParams(stpos+s, v1, lmperiod, lmapo, lmtta, lmper, lmttp);
		// do some sanity checking - if transfer gets too low, don't do it
		if(lmttp < lmtta) {
			if (lmper < 18461.0) {
				// try again later...
				BurnTime=BurnTime+30.0*60.0;
				ProgState=2;
				return;
			}
		}

		dv1=v1-stvel;
		DesiredDeltaVx=dv1.x;
		DesiredDeltaVy=dv1.y;
		DesiredDeltaVz=dv1.z;
//			fprintf(outstr, "dv=%.3f %.3f %.3f mag=%.3f \n", dv1, Mag(dv1));
		if(ApolloNo < A14ASCENT) {
			//P41 uses RCS
			BurnTime=BurnTime-(vmass/thrust)*Mag(dv1);
		} else {
			//P42 uses APS
			fmass=vmass*(1.0-exp(-(Mag(dv1)/visp)));
			BurnTime=BurnTime-(fmass/(thrust/visp));
//				fprintf(outstr, "th=%.1f isp=%.1f fmass=%.1f vmass=%.1f\n",
//					thrust, visp, fmass, vmass);
		}
		BurnStartTime=BurnTime;
		BurnEndTime=BurnStartTime+t;

//			fprintf(outstr, "v1 = %.3f %.3f %.3f stvel=%.3f %.3f %.3f \n", v1, dv1);
//			fprintf(outstr, "ddv= %.3f %.3f %.3f at simt=%.1f to %.1f\n", DesiredDeltaVx,
//				DesiredDeltaVy, DesiredDeltaVz, BurnStartTime, BurnEndTime);
		dt=DeltaPitchRate-BurnEndTime;
		PredictPosVelVectors(stpos, v1, mu, dt, rpos, rvel, velm);
//			fprintf(outstr, "pred pos=%.1f %.1f %.1f rvel=%.1f %.1f %.1f \n",rpos, rvel);

		PredictPosVelVectors((stpos+s), v1, mu, dt, rpos, rvel, velm);
//			fprintf(outstr, "disp pos=%.1f %.1f %.1f rvel=%.1f %.1f %.1f \n",rpos, rvel);

//			fprintf(outstr, "rend pos=%.1f %.1f %.1f rvel=%.1f %.1f %.1f \n",renpos, renvel);
//			fclose(outstr);
	}

	if(ProgState == 1) {
//		SetVerbNounAndFlash(6,37);
		SetVerbNoun(16, 39);
		//use Realism when it works
		ProgState++;
	}
	if(ProgState == 3) {
//		SetVerbNounAndFlash(6,81);
		//use Realism when it works
		ProgState++;
	}

	if(ProgState == 4) {
		if(simt > NextEventTime) {
			NextEventTime=simt+DELTAT;
			OBJHANDLE hbody=OurVessel->GetGravityRef();

			OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
			VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
			CSMVessel->GetRelativePos(hbody, csmpos);
			OurVessel->GetRelativePos(hbody, lmpos);
			vec=Normalize(csmpos-lmpos);
			OrientAxis(vec, 2, 0);
		}
//		sprintf(oapiDebugString(), "TPI time: %.1f", BurnStartTime-simt);
		if(simt > BurnStartTime-120.0) {
			ProgState=0;
			DesiredLAN=35;
			DesiredPlaneChange=1.0;
			if(ApolloNo < A14ASCENT) {
				RunProgram(41);
			} else {
				RunProgram(42);
			}
			return;
		}
	}
}

void LEMcomputer::Prog34Pressed(int R1, int R2, int R3)
{
	switch (ProgState)
	{
	case 1:
		ProgState++;
		return;

	case 3:
		ProgState++;
		return;
	}
	LightOprErr();
}

//
//	TPM - Terminal Phase Midcourse program
//

void LEMcomputer::Prog35(double simt)
{
	const double GRAVITY=6.67259e-11;
	VECTOR3 lmpos, lmvel, stpos, stvel, v1, v2, csmpos, csmvel, crpos, crvel,
		lrpos, lrvel, dv, vec;
	double dt, mu, rtime, time, ctime, velm, vmass, ythrust;
	int i, nt;

//			char fname[8];
//			sprintf(fname,"P35log.txt");
//			outstr=fopen(fname,"w");
//			fprintf(outstr, "P35: ProgState=%d\n", ProgState);
	LightCompActy();
	if(simt > NextEventTime) {
		NextEventTime+=DELTAT;
		if(ProgState == 0) {
			ProgState++;
			OBJHANDLE hbody=OurVessel->GetGravityRef();
			double bradius=oapiGetSize(hbody);
			double bmass=oapiGetMass(hbody);
			mu=GRAVITY*bmass;

			OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
			VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
			CSMVessel->GetRelativePos(hbody, csmpos);
			CSMVessel->GetRelativeVel(hbody, csmvel);
			// rendezvous time - seconds from NOW
			rtime=DeltaPitchRate-simt;
			PredictPosVelVectors(csmpos, csmvel, mu, rtime, crpos, crvel, velm);
			OurVessel->GetRelativePos(hbody, lmpos);
			OurVessel->GetRelativeVel(hbody, lmvel);
			vmass=OurVessel->GetMass();
			PredictPosVelVectors(lmpos, lmvel, mu, rtime, lrpos, lrvel, velm);

//			fprintf(outstr," miss=%.1f dv=%.1f in %.1f secs\n", Mag(crpos-lrpos),
//				Mag(crvel-lrvel), rtime);
			if(rtime > 30.0*60.0) {
				ctime=DeltaPitchRate-30.0*60.0;
				DesiredLAN=35;
			} else {
				ctime=DeltaPitchRate-15.0*60.0;
				DesiredLAN=36;
			}
			time=ctime-simt;
			PredictPosVelVectors(lmpos, lmvel, mu, time, stpos, stvel, velm);

//			fprintf(outstr, "Corr pos: %.1f %.1f %.1f vel=%.1f %.1f %.1f time=%.1f\n",
//				stpos, stvel, time);
			dt=rtime-time;
			Lambert(stpos, crpos, dt, mu, v1, v2);
//			fprintf(outstr, "v1=%.3f %.3f %.3f dv=%.3f %.3f %.3f time=%.1f\n",
//				v1, v1-stvel, dt);
			dv=v1-stvel;
//			fprintf(outstr," mag before=%.3f mag after=%.3f\n", Mag(stvel), Mag(v1));
			DesiredDeltaVx=dv.x;
			DesiredDeltaVy=dv.y;
			DesiredDeltaVz=dv.z;
			ythrust=0.0;
			THRUSTER_HANDLE th;
			nt=OurVessel->GetGroupThrusterCount(THGROUP_ATT_UP);
			for (i=0; i<nt; i++) 
			{
				th=OurVessel->GetGroupThruster(THGROUP_ATT_UP, i);
				ythrust=ythrust+OurVessel->GetThrusterMax(th);
			}
			BurnTime=((vmass/ythrust)*Mag(dv));
			if(BurnTime < 1.0) {
				DesiredPlaneChange=0.1;
				BurnTime=10.0*BurnTime;
			} else {
				DesiredPlaneChange=1.0;
			}
			BurnStartTime=ctime-BurnTime;
			BurnEndTime=BurnStartTime+BurnTime;
			SetVerbNoun(16, 39);
//			fclose(outstr);
		}
		if(ProgState == 1) {
			OBJHANDLE hbody=OurVessel->GetGravityRef();

			OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
			VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
			CSMVessel->GetRelativePos(hbody, csmpos);
			OurVessel->GetRelativePos(hbody, lmpos);
			vec=Normalize(csmpos-lmpos);
			OrientAxis(vec, 2, 0);
//			sprintf(oapiDebugString(), "TPM time=%.1f", BurnStartTime-simt);
			if(BurnStartTime-simt < 120.0)	{
				RunProgram(41);
				return;
			}
		}
	}
}

// This is a fictitious program to do the final rendezvous braking and station keeping

void LEMcomputer::Prog36(double simt)
{
	static VECTOR3 zero = {0.0, 0.0, 0.0};
	VECTOR3 csmpos, lmpos, csmvel, lmvel, vec, rvel, rpos, zerogl, xgl, ygl, zgl, level, 
		rlcl, vlcl;
	double dis, vel, dv, vmass, zo, yo, acc;
	int axis;

	LightCompActy();
	if(simt > NextEventTime) {
		NextEventTime=simt+DELTAT_P36;

		if (ProgState == 0) {
			BurnStartTime=DeltaPitchRate+200.0;
			SetVerbNoun(16, 54);
			ProgState++;
			ProgFlag01=false;
			ProgFlag02=false;
			CommandedAttitudeLinLevel = zero;
			CommandedAttitudeRotLevel = zero;
			LastEventTime = simt;
		}
		if(ProgState >= 1) {
			OBJHANDLE hbody=OurVessel->GetGravityRef();

			OBJHANDLE hcsm=oapiGetVesselByName(OtherVesselName);
			VESSEL *CSMVessel=oapiGetVesselInterface(hcsm);
			CSMVessel->GetRelativePos(hbody, csmpos);
			OurVessel->GetRelativePos(hbody, lmpos);
			CSMVessel->GetRelativeVel(hbody, csmvel);
			OurVessel->GetRelativeVel(hbody, lmvel);
			rpos=lmpos-csmpos;
			rvel=lmvel-csmvel;
			vel=Mag(rvel);
			dis=Mag(csmpos-lmpos);
			vec=Normalize(csmpos-lmpos);
			vmass=OurVessel->GetMass();
			acc=(vel*vel)/(2.0*dis);
			if(dis >= 80000.0 || ((simt - LastEventTime) < 60)) {
				DesiredDeltaVz=vel;
			} else {
				ProgFlag01=true;
				if(dis < 80000.0) DesiredDeltaVz=100.0;
				if(dis < 40000.0) DesiredDeltaVz=60.0;
				if(dis < 20000.0) DesiredDeltaVz=40.0;
				if(dis < 10000.0) DesiredDeltaVz=27.0;
				if(dis < 6000.0) DesiredDeltaVz=20.0;
				if(dis < 4000.0) DesiredDeltaVz=15.0;
				if(dis < 1846.0) DesiredDeltaVz=9.23;
				if(dis < 923.0) DesiredDeltaVz=6.15;
				if(dis < 461.5) DesiredDeltaVz=3.076;
				if(dis < 184.6) DesiredDeltaVz=1.538;
				if(dis < 30.0 ) DesiredDeltaVz=0.75;
				if(dis < 20.0) 	ProgFlag02=true;
			}
			dv = (rvel * vec) - DesiredDeltaVz;
			if(dv > 0.0) {
				BurnTime=((vmass/(TRTHRUST_P36/2.0))*dv);
				BurnEndTime=simt+((vmass/(TRTHRUST_P36/2.0))*dv);
			}
			axis=2;
			if(ProgFlag01) {				
				OurVessel->Local2Rel(_V(0.0, 0.0, 0.0), zerogl);
				OurVessel->Local2Rel(_V(1.0, 0.0, 0.0), xgl);
				OurVessel->Local2Rel(_V(0.0, 1.0, 0.0), ygl);
				OurVessel->Local2Rel(_V(0.0, 0.0, 1.0), zgl);

				xgl=xgl-zerogl;
				ygl=ygl-zerogl;
				zgl=zgl-zerogl;
				vlcl.x=rvel*xgl;
				vlcl.y=rvel*ygl;
				vlcl.z=rvel*zgl;
				rlcl.x=rpos*xgl;
				rlcl.y=rpos*ygl;
				rlcl.z=rpos*zgl;				
				if(ProgState == 1) {
					zo=15.0;
					yo=0.0;
				} else {
					zo=0.0;
					yo=15.0;
				}

				if (fabs(vlcl.x) > 0.0001) {
					level.x = -(((rlcl.x * min(0.1, 100.0 / dis)) + vlcl.x) * vmass) / (TRTHRUST_P36 * DELTAT_P36);
				} else {
					level.x = 0.0;
				}
				level.x = max(-1, min(1, level.x));

				if (fabs(vlcl.y) > 0.0001) {
					level.y = -((((rlcl.y + yo) * min(0.1, 100.0 / dis)) + vlcl.y) * vmass) / (TRTHRUST_P36 * DELTAT_P36);					
				} else {
					level.y=0.0;
				}
				level.y = max(-1, min(1, level.y));

				if(ProgState == 2) {
					if(simt < BurnTime) {
						level.x=0.0;
						level.y=0.0;
					}
				}
				CommandedAttitudeLinLevel.x = level.x;
				CommandedAttitudeLinLevel.y = level.y;
					
				level.z=0.0;
				if(ProgFlag02) {
					if(fabs(vlcl.z) > 0.0001) {
						level.z=-((((rlcl.z+zo)/10.0)+vlcl.z)*vmass)/(TRTHRUST_P36*DELTAT_P36);
						if(level.z > 1.0)  level.z=1.0;
						if(level.z < -1.0) level.z=-1.0;
					} else {
						level.z=0.0;
					}
					if(ProgState == 2) {
						if(simt < BurnTime) {
							level.z=0.0;
						}
					}
					CommandedAttitudeLinLevel.z = level.z;

					if((fabs(rlcl.z+15.0) < 0.1) && fabs(vlcl.z) < 0.001) {
						ProgState=2;
						ProgFlag01=false;
						ProgFlag02=false;
						CommandedAttitudeLinLevel = zero;

//						BurnEndTime=simt+60.0;
						BurnTime=simt+60.0;
					}

				}
//				sprintf(oapiDebugString(), "rlcl=%.3f %.3f %.3f vlcl=%.3f %.3f %.3f lvl=%.3f %.3f %.3f", 
//					rlcl, vlcl, level);
			}
			if(ProgState == 2) axis=1;

			double gainFactor = 1.0;
			if(ProgFlag02 == false && simt < BurnEndTime) gainFactor = 10.0;
			
			CommandedAttitudeRotLevel = OrientAxis(vec, axis, 0, gainFactor);
				
			if(axis == 1) {
				if(simt > BurnTime) {
					ProgFlag01=true;
					ProgFlag02=true;
//					RunProgram(0);
//					return;
				}
			}

//			sprintf(oapiDebugString(),"State=%d f1=%d f2=%d axis=%d vec=%.3f %.3f %.3f", 
//				ProgState, ProgFlag01, ProgFlag02, axis, vec);

		}
	} //end of 2 sec guidance loop
	if(ProgFlag02 == false) {
		if(simt < BurnEndTime) {
			CommandedAttitudeLinLevel.z = -1;
			CommandedAttitudeLinLevel.x = 0;
			CommandedAttitudeLinLevel.y = 0;
		} else {
			CommandedAttitudeLinLevel.z = 0;
		}
	}
	SetAttitudeRotLevel(CommandedAttitudeRotLevel);
	AddAttitudeLinLevel(CommandedAttitudeLinLevel);
	// sprintf(oapiDebugString(),"Rot x %.3f y %.3f z %.3f Lin x %.3f y %.3f z %.3f dv %.3f BT %.3f Axis %d", CommandedAttitudeRotLevel.x, CommandedAttitudeRotLevel.y, CommandedAttitudeRotLevel.z,
	//	CommandedAttitudeLinLevel.x, CommandedAttitudeLinLevel.y, CommandedAttitudeLinLevel.z, dv, BurnEndTime - simt, axis);

}
