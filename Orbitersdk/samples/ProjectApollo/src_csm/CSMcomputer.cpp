/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Saturn CSM computer

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
#include "nasspsound.h"
#include "nasspdefs.h"

#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"
#include "toggleswitch.h"
#include "saturn.h"
#include "ioChannels.h"
#include "papi.h"
#include "thread.h"

CSMcomputer::CSMcomputer(SoundLib &s, DSKY &display, DSKY &display2, IMU &im, PanelSDK &p, CSMToIUConnector &i, CSMToSIVBControlConnector &sivb) : 
	ApolloGuidance(s, display, im, p), dsky2(display2), iu(i), lv(sivb)

{
	isLGC = false;
	BurnTime = 0;
	BurnStartTime = 0;
	CutOffVel = 0;
	lastOrbitalElementsTime = 0;

	VesselStatusDisplay = 0;

	MainThrusterIsHover = false;

	FlagWord0.word = 0;
	FlagWord1.word = 0;
	FlagWord2.word = 0;
	FlagWord3.word = 0;
	FlagWord4.word = 0;
	FlagWord5.word = 0;

	//
	// Set sensible values for flags.
	//

	FlagWord0.u.P29FLAG = 1;
	FlagWord0.u.IMUSE = 1;
	FlagWord1.u.AVEGFLAG = 1;
	FlagWord1.u.TRACKFLG = 1;
	FlagWord1.u.UPDATFLG = 1;
	FlagWord2.u.CALCMAN2 = 1;
	FlagWord3.u.VINTFLAG = 1;
	FlagWord3.u.V50N18FL = 1;
	FlagWord5.u.DSKYFLAG = 1;

	//
	// Generic thrust decay value. This still needs tweaking.
	//

	ThrustDecayDV = 6.1;

	//
	// Last RCS settings.
	//

	LastOut5 = 0;
	LastOut6 = 0;
	LastOut11 = 0;

// autopilot variables
	GONEPAST = false;
	EGSW = false;
	HIND = false;
	LATSW = false;
	RELVELSW = false;
	INRLSW = false;
	GONEBY = false;
	NOSWITCH = false;
	ACALC = 0;
	RollCode = 0;
	K2ROLL = 0;
	SELECTOR = 0;
	KA = 0;
	Dzero = 0;
	LoverD = 0;
	Q7 = 0;
	FACTOR = 0;
	FACT1 = 0;
	FACT2 = 0;
	DIFFOLD = 0;
	LEWD = 0;
	DLEWD = 0;
	A0 = 0;
	A1 = 0;
	VS1 = 0;
	ALP = 0;
	AHOOK = 0;
	DHOOK = 0;
	V1 = 0;

// DSKY display variables
	V = 0;
	D = 0;
	PREDGMAX = 0;
	VPRED = 0;
	GAMMAEI = 0;
	TIME5 = 0;
	RANGETOGO = 0;
	RTOSPLASH = 0;
	TIMETOGO = 0;
	VIO = 0;
	ROLLC = 0;
	LATANG = 0;
	XRNGERR = 0;
	DNRNGERR = 0;
	RDOT = 0;
	VL = 0;

	thread.Resume ();
}

CSMcomputer::~CSMcomputer()

{
	//
	// Nothing for now.
	//
}

void CSMcomputer::SetMissionInfo(int MissionNo, int RealismValue, char *OtherVessel)

{
	ApolloGuidance::SetMissionInfo(MissionNo, RealismValue, OtherVessel);

	//
	// Pick the appropriate AGC binary file based on the mission number.
	//

	char *binfile = "Config/ProjectApollo/Artemis072.bin";
	if (MissionNo < 15 || MissionNo == 1301)	// same criterium in CSMcomputer::Timestep because of pad load
		binfile = "Config/ProjectApollo/Colossus249.bin";

	InitVirtualAGC(binfile);
}

//
// Validate verb and noun combinations when they are
// entered on the DSKY, for verbs specific to this
// class of computer.
//

bool CSMcomputer::ValidateVerbNoun(int verb, int noun)

{
	switch (verb) {

	case 21:
		if (ProgRunning == 15 && (noun == 14 || noun == 33))
			return true;
		return false;

	case 75:
	case 82:
		return true;

	case 46:
		if (ProgRunning == 11)
			return true;
		return false;
	case 37:
		if(noun == 61)
			return true;
		return false;

	}

	return false;
}


//
// CalcTFF calculates the freefall time from our current position to
// entry at 300,000 feet. This assumes that g is constant, so is only
// valid in a fairly circular orbit.. this is a good approximation for
// launch and entry, since g won't change much over the course of the
// time we're interested in.
//

const double tff_alt = (300000 * 0.3048);	// 300,000 feet.

double CSMcomputer::CalcTFF(double vy, double r0, double g)

{
	double c = (vy * vy) + (2 * g * (r0 - tff_alt));

	//
	// If we're never going to reach 300,000 feet then return max time.
	//

	if (c <= 0 || g <= 0)
		return 3600.0;

	//
	// Otherwise calculate the tff.
	//

	double tff =  (vy + sqrt(c)) / g;

	//
	// If the time is negative, we've already dropped below 300,000 feet,
	// so return max time.
	//

	if (tff < 0)
		return 3600.0;

	return tff;
}

//
// Actually process the verb and noun.
//

void CSMcomputer::DisplayNounData(int noun)

{
	//
	// CSM-specific overrides
	//

	switch (noun)
	{
	case 63:
		if (ProgRunning == 63)
		{
			int R1,R2;
			R1 = (int) (10.0 * RANGETOGO);
			R2 = (int) (VIO);
			int	min = (int) (TIME5 / 60);
			int sec = (int) (TIME5 - (min * 60));
			SetR3(-(min * 1000 + sec));
			SetR3Format(TwoSpaceTwoFormat);
			SetR1(R1);
			SetR2(R2);
		}
		break;
	case 64:
		if ((ProgRunning >= 63)&&(ProgRunning <= 64))
		{
			int R1,R2,R3;
			R1 = (int) (10.2 * D + 0.5);
			R2 = (int) (V);
			R3 = (int) (10.0 * RTOSPLASH);
			SetR1(R1);
			SetR2(R2);
			SetR3(R3);
		}
		break;
	case 66:
		if ((ProgRunning == 64)&&(ProgState == 9))
		{
			int R1,R2,R3;
			R1 = (int) (5730.0 * ROLLC);
			R2 = (int) (10.0 * XRNGERR);
			R3 = (int) (10.0 * DNRNGERR);
			SetR1(R1);
			SetR2(R2);
			SetR3(R3);
		}
		break;

	case 68:
		if ((ProgRunning >= 63)&&(ProgRunning <= 64))
		{
			int R1,R2,R3;
			R1 = (int) (5730.0 * ROLLC);
			R2 = (int) (V);
			R3 = (int) (RDOT);
			SetR1(R1);
			SetR2(R2);
			SetR3(R3);
		}
		break;
	case 69:
		if((ProgRunning == 64)&&(ProgState == 5))
		{
			int R1,R2,R3;
			R1 = (int) (5730.0 * ROLLC);
			R2 = (int) (100.0 * Q7 / GS);
			R3 = (int) (V1);
			SetR1(R1);
			SetR2(R2);
			SetR3(R3);
		}
		break;
	case 74:
		if ((ProgRunning >= 63)&&(ProgRunning <= 64))
		{
			int R1,R2,R3;
			R1 = (int) (5730.0 * ROLLC);
			R2 = (int) (V);
			R3 = (int) (10.2 * D + 0.5);
			SetR1(R1);
			SetR2(R2);
			SetR3(R3);
		}
		break;

	//
	// 44: Orbit parameters. Only available during program 11.
	//

	case 44:
		if (ProgRunning == 11)
		{
			ELEMENTS el;
			double mjd_ref;

			OBJHANDLE hbody = OurVessel->GetGravityRef();
			double bradius = oapiGetSize(hbody);
			OurVessel->GetElements(el, mjd_ref);

			double apogee = (el.a * (1.0 + el.e)) - (bradius);
			double perigee = (el.a * (1.0 - el.e)) - (bradius);

			if (apogee < 0)
				apogee = 0;

			SetR1((int)DisplayAlt(apogee) / 100);
			SetR2((int)DisplayAlt(perigee) / 100);

			//
			// Adjust gravity to take into account our current velocity parallel to the planet's
			// surface.
			//

			double g = CurrentG() - ((CurrentVelX * CurrentVelX + CurrentVelZ * CurrentVelZ) / (CurrentAlt + bradius));
			double tff = CalcTFF(CurrentVelY, CurrentAlt, g);

			int	min = (int)(tff / 60);
			int sec = (int)tff - (min * 60);

			if (min > 59) {
				min = 59;
				sec = 59;
			}

			SetR3(min * 1000 + sec);
			SetR3Format(TwoSpaceTwoFormat);
			return;
		}
		break;
	}

	if (DisplayCommonNounData(noun))
		return;

	switch (noun) {

	//
	// 14: desired delta V
	//

	case 14:
		SetR1((int)DisplayVel(DesiredDeltaV));
		break;

	//
	// 62: inertial velocity information. Velocity, altitude rate, altitude.
	//

	case 62:
		SetR1((int)DisplayVel(CurrentRVel));
		SetR2((int)DisplayVel(CurrentVelY));
		SetR3((int)(DisplayAlt(CurrentAlt) / 1000));
		break;

	case 39:
		SetR1(VesselStatusDisplay);
		break;

	case 76:
		switch (VesselStatusDisplay) 
		{
		case 1:
			{
				//
				// Fuel mass in kg.
				//
				double FMass = lv.GetFuelMass();
				SetR2((int) FMass);
			}
			break;

		case 2:
			{
				double capacity, drain;
				lv.GetMainBatteryPower(capacity, drain);

				//
				// Battery power in kWh x 10.
				//
				SetR2((int) (capacity / 360000.0));
				//
				// Power drain in watts.
				//
				SetR3((int) drain);
			}
			break;

		//
		// 3: SIVb battery voltage and current.
		//
		case 3:
			{
				double volts, current;
				lv.GetMainBatteryElectrics(volts, current);

				//
				// Voltage x 100.
				//
				int vi = (int) (volts * 100.0);
				int vd100 = vi / 100;
				int vR2 = (vd100 * 1000) + (vi - (vd100 * 100));

				SetR2(vR2);
				SetR2Format(TwoSpaceTwoFormat);

				//
				// Current x 100.
				//
				int ci = (int) (current * 100.0);
				int cd100 = ci / 100;
				int cR3 = (cd100 * 1000) + (ci - (cd100 * 100));

				//
				// Sanity check.
				//
				if (current > 99.99)
				{
					cR3 = 99099;
				}

				//
				// Display it.
				//
				SetR3(cR3);
				SetR3Format(TwoSpaceTwoFormat);
			}
			break;
		}
		break;

	//
	// 73: altitude, velocity, AoA
	//

	case 73:
		if (!Reset) {
			BlankData();
			return;
		}

		SetR1((int)(DisplayAlt(CurrentAlt) / 1000));
		SetR2((int)DisplayVel(CurrentVel));
		SetR3((int)(OurVessel->GetAOA() * 5729.57795));
		break;
	}
}

//
// Are the apogee/perigee orbit calculations valid?
//

bool CSMcomputer::OrbitCalculationsValid()

{
	switch (ProgRunning)
	{
	case 11:
	case 15:
		return true;

	case 37:
		if (ProgState >= 2)
			return true;
		break;
	}

	return false;
}

void CSMcomputer::ProcessVerbNoun(int verb, int noun)

{
	switch (verb) {

	//
	// 46: disable autopilot (actually toggle here).
	//

	case 46:
		if (ProgRunning == 11) {
			ChannelValue12 val12;
			val12.Value = GetOutputChannel(012);

			val12.Bits.EnableSIVBTakeover = !val12.Bits.EnableSIVBTakeover;
			SetOutputChannel(012, val12.Value);
		}
		break;
	case 37:
		if(noun == 61){
			RunProgram(61);
			Prog = 61;
			SetVerbNounAndFlash(6,61);
		}
		break;

	//
	// 75: launch.
	//

	case 75:
		Liftoff(LastTimestep);
		break;

	//
	// 82: display orbit parameter settings.
	//

	case 82:
		if (ProgRunning == 11) {
			SetVerbNounAndFlash(16, 44);
		}
	}
}

//
// Validate program number for programs specific to this
// class of computer.
//

bool CSMcomputer::ValidateProgram(int prog)

{
	if (FlagWord2.u.NODOFLAG)
		return false;

	switch (prog) {

	case 1:
	case 2:
	case 6:
	case 11:
		return true;

	case 15:
		return iu.IsTLICapable();

	case 59:
		return lv.IsVentable();
	}

	return false;
}

//
// Program 01: pre-launch IMU align.
//

void CSMcomputer::Prog01(double simt)

{
	switch (ProgState) {

	case 0:

		if (FlagWord1.u.NODOP01) {
			AbortWithError(021521);
			break;
		}

		LightCompActy();
		LightNoAtt();

		// Check if the IMU is running and uncaged.
		ChannelValue30 val30;
		val30.Value = GetInputChannel(030);
		if (val30.Bits.IMUCage || !val30.Bits.IMUOperate) {
			AbortWithError(0210);
			break;
		}

		SetOutputChannelBit(012, 5, true);
		SetOutputChannelBit(012, 5, false);
		SetOutputChannelBit(012, 15, true);

		NextProgTime = simt + 2.0;
		ProgState++;
		break;

	case 1:
		if (simt >= NextProgTime) {
			LightCompActy();

			// Check if the IMU is running and uncaged.
			val30.Value = GetInputChannel(030);
			if (!Yaagc && val30.Bits.IMUOperate && !val30.Bits.IMUCage) {

				//
				// As a quick hack we drive the IMU to prelaunch orientation, 
				//

				double heading;
				oapiGetHeading(OurVessel->GetHandle(), &heading);
				imu.DriveGimbals(TWO_PI - heading + (DesiredAzimuth / DEG), 90.0 / DEG, 0.0);
			}
			NextProgTime = simt + 1.0;
			ProgState++;
		}
		break;

	case 2:
		if (simt >= NextProgTime) {
			ChannelValue30 val30;

			LightCompActy();
			// Check if the IMU is running and uncaged.
			val30.Value = GetInputChannel(030);
			if (val30.Bits.IMUOperate && !val30.Bits.IMUCage) {
				ClearNoAtt();
				RunProgram(2);
			}
			else 
				NextProgTime = simt + 1.0;
		}
		break;
	}
}

//
// Program 02: launch setup.
//

void CSMcomputer::Prog02(double simt)

{
	switch (ProgState) {

	case 0:
		LightCompActy();
		SetVerbNounAndFlash(6, 29);
		ProgState++;
		break;

	case 2:
		LightCompActy();
		SetVerbNounAndFlash(6, 44);
		ProgState++;
		break;

	case 4:
		LightCompActy();
		SetVerbNounAndFlash(6, 33);
		ProgState++;
		break;
	}

	// Check if the IMU is running and uncaged.
	ChannelValue30 val30;
	val30.Value = GetInputChannel(030);
	if (!Yaagc && val30.Bits.IMUOperate && !val30.Bits.IMUCage) {

		//
		// As a quick hack we do "gyro-compassing" to maintain prelaunch orientation, 
		//

		double heading;
		oapiGetHeading(OurVessel->GetHandle(), &heading);
		imu.DriveGimbals(TWO_PI - heading + (DesiredAzimuth / DEG), 90.0 / DEG, 0.0);
	}
}


//
// Data entry for Prog 02.
//

void CSMcomputer::Prog02Pressed(int R1, int R2, int R3)

{
	switch (ProgState) {

	case 1:
		//
		// Waiting for launch azimuth.
		//

		if (R1 < 0 || R1 > 35999) {
			LightOprErr();
			return;
		}

		DesiredAzimuth = ((double) R1) / 100.0;
		ProgState++;
		break;

	case 3:
		//
		// Waiting for apogee and perigee.
		//

		DesiredApogee = ((double) R1) / 100.0;
		DesiredPerigee = ((double) R2) / 100.0;
		ProgState++;
		break;

	case 5:
		//
		// Waiting for burn time.
		//

		{
			Saturn *sat = (Saturn *) OurVessel;
			UpdateBurnTime(R1, R2, R3);
			sat->UpdateLaunchTime(BurnTime - LastTimestep);
			ProgState = 4;
		}
		break;

	default:
		//
		// Anything else is an error
		//

		LightOprErr();
		break;
	}
}

//
// Program 06: standby.
//

void CSMcomputer::Prog06(double simt)

{
	switch (ProgState) {

	case 0:
		Checklist(062);
		ProgState++;
		break;
	}
}

//
// Program 11: main launch program.
//

void CSMcomputer::Prog11(double simt)

{
	Saturn *Sat = (Saturn *)OurVessel;

	switch (ProgState) {

	case 0:
		SetVerbNoun(16, 62);

		LightCompActy();

		//
		// Clear flash in case this program was auto-started by
		// the launch.
		//

		ClearVerbNounFlashing();

		ProgState++;
		break;

	case 1:

		//
		// If we've reached orbit, turn off the autopilot and indicate to
		// the user that they should select a new program.
		//

		if (InOrbit) {

			VerbRunning = 0;
			NounRunning = 0;

			InOrbit = 1;

			Sat->SetAutopilot(false);

			AwaitProgram();

			ProgState++;
		}
		break;

	};
}

// Reentry programs P61 - P67

void CSMcomputer::Prog61(double simt)
{
	int R1, R2, R3, min, sec;
	double smaj, ec, pl, mjd, rad, theta, ctheta, stheta, lati, longi;
	double nu, nu1, manom, eanom, time, hx, hy, hz, hm, energy, rx, ry, rz;
	double longitude, latitude, radius, xpx, xpy, xpz;
	double zpx, zpy, zpz, rentry5, v2;
	VECTOR3	rentry;
	ELEMENTS elem;
	VESSELSTATUS status;

	if(simt > NextEventTime)
	{
		OurVessel->GetStatus(status);
		rad = sqrt(status.rpos.x * status.rpos.x + status.rpos.y * status.rpos.y + status.rpos.z * status.rpos.z);
		xpx = status.rpos.x / rad;
		xpy = status.rpos.y / rad;
		xpz = status.rpos.z / rad;
		V = sqrt(status.rvel.x * status.rvel.x + status.rvel.y * status.rvel.y + status.rvel.z * status.rvel.z);
// calculate angular momentum vector hx, hy, hz; h = r x v
		hx = -(status.rpos.y * status.rvel.z - status.rpos.z * status.rvel.y);
		hy = -(status.rpos.z * status.rvel.x - status.rpos.x * status.rvel.z);
		hz = -(status.rpos.x * status.rvel.y - status.rpos.y * status.rvel.x);
// normalize
		hm = sqrt(hx * hx + hy * hy + hz * hz);
		hx = hx / hm;
		hy = hy / hm;
		hz = hz / hm;
		zpx= -(hy * xpz - hz * xpy);
		zpy= -(hz * xpx - hx * xpz);
		zpz= -(hx * xpy - hy * xpx);
// We have made a new coordinate system with unit axes (in ecliptic coords):
//		X' = (xpx, xpy, xpz) in the direction of the current position vector.
//		Y' = (hx, hy, hz) in the direction of the angular momentum vector.
//		Z' = X' x Y'
// in this system, our current position is (rad, 0,  0). 
// We will use it to calculate the entry and splashdown points from true anomalies.
		int i=0;
		switch (ProgState)
		{
		case 0:
			BlankData();
			SetVerbNounAndFlash(6, 61);
// R1 = Lat, R2 = Long, R3 = heads-up (+1) or down (-1)
			OurVessel->ActivateNavmode(NAVMODE_KILLROT);
			OurVessel->DeactivateNavmode(NAVMODE_KILLROT);
			for(i=1;i<9;i++)
			{
				SetOutputChannelBit(05,i,false);
				SetOutputChannelBit(06,i,false);
			}
// calculate predicted V at 122km
			energy = V * V / 2.0 - MUE / rad;
			VPRED = sqrt(2.0 * (energy + MUE / (RE + 122000.0)));
// calculate entry angle
			if ((hm / ((RE + 122000.0) * VPRED)) > 1.0) {
				LightOprErr();
				return;
			}
			GAMMAEI = -acos(hm / ((RE + 122000.0) * VPRED));
// calculate a nominal splashdown point, RANGETOGO is downrange distance from
//		0.05G to splashdown. Currently derived from the real ranges flown by Apollo 7 and 11
// entry range and entry time
			RANGETOGO = VPRED/4.8;
			if(VPRED < 7925.0) TIMETOGO = 1427.0 * RANGETOGO / VPRED;
			else TIMETOGO = RANGETOGO / 5.55;
// calculate the current true anomaly and the true anomaly for 0.05G alt.
			OurVessel->GetElements(elem,mjd);
			smaj = elem.a;
			ec = elem.e;
			pl = smaj * (1.0 - ec * ec);
			nu = acos((pl / rad - 1.0) / ec);
// estimate for the entry radius (0.05G)
			rentry5 = RE + 86000.0 + 0.94 * (VPRED - 7800.0);
			nu1 = acos((pl / rentry5 - 1.0) / ec);

// add the estimated range for entry to splashdown
			theta = nu - nu1 + 1000.0 * RANGETOGO / RE;
// rotate current normalized position vector (1,0,0) by theta about Y' vector.
// the result is a normalized position vector for the splashdown point.
			ctheta = cos(theta);
			stheta = sin(theta);
			rx = ctheta;
			ry = 0.0;
			rz = stheta;
// calculate the coordinates in ecliptic system
			rentry.x = rx * xpx + ry * hx + rz * zpx;
			rentry.y = rx * xpy + ry * hy + rz * zpy;
			rentry.z = rx * xpz + ry * hz + rz * zpz;
// finally rotate from ecliptic to equatorial coordinates (23.45 degrees)
			ctheta = 0.9174077;
			stheta = -0.3979486;
			rx = rentry.x;
			ry = rentry.y * ctheta - rentry.z * stheta;
			rz = rentry.z * ctheta + rentry.y * stheta;
			lati = asin(ry);
			longi = atan(rz / rx);
			if(rx < 0.0) longi = 3.14159 + longi;
// add the Earth's current rotation
			theta = oapiGetPlanetCurrentRotation (OurVessel->GetSurfaceRef());
			longi = longi - theta;
			OurVessel->GetEquPos(longitude, latitude, radius);
// allow for the Earth to rotate between now and when we get there
			manom = acos((1.0 - rad / smaj) / ec);
			manom = manom - ec * sin(manom);
			eanom = acos((1.0 - rentry5 / smaj) / ec);
			eanom = eanom - ec * sin(eanom);
			time = sqrt(smaj * smaj * smaj / MUE) * (manom - eanom);
// add in the reentry time
			time += TIMETOGO;
			theta = 0.00417807 * time; // Earth rotates 0.00417807 degrees per second, est. 500 sec for reentry
			longi = 57.2958 * longi - theta;
			lati = 57.2958 * lati;
			while(longi < -180.0) longi += 360.0;
			while(longi > 180.0) longi -= 360.0;
			R1 = (int) ((lati + 0.005) * 100.0);
			R2 = (int) ((longi + 0.005) * 100.0);
			lati = R1 / 100.0;
			longi = R2 / 100.0;
			SetDesiredLanding(lati, longi, 0.0);
			RollCode = -1;
			R3 = RollCode;
			SetR1(R1);
			SetR2(R2);
			SetR3(R3);
			ProgState++;
			break;

		case 2:
			BlankData();
			SetVerbNounAndFlash(6, 60);
// R1 = G_max, R2 = V_pred (at 400k ft), R3 = gamma (entry angle)

// the pilot may have entered a new landing point, and also may have done a maneuver, 
//       so we need to calculate everything from scratch
// calculate VPRED
			energy = V * V / 2.0 - MUE / rad;
			VPRED = sqrt(2.0 * (energy + MUE / (RE + 122000.0)));
// calculate entry angle
			GAMMAEI = -acos(hm / ((RE + 122000.0) * VPRED));
// calculation for estimating G_max
// this calculation is really only good for lunar trajectory, but is still approx. correct
// for orbital entry
			GAMMAEI = 57.3*GAMMAEI;
			if(fabs(GAMMAEI) < 5.9) PREDGMAX = 4.0 + fabs(GAMMAEI) / 6.0;
			else if(fabs(GAMMAEI) < 6.5) PREDGMAX = -12.717 + 3.0 * fabs(GAMMAEI);
			else PREDGMAX = -32.21 + 6.0 * fabs(GAMMAEI);
			R1 = (int) (PREDGMAX * 100.0);
			R2 = (int) (VPRED);
			R3 = (int) (GAMMAEI * 100.0);
			SetR1(R1);
			SetR2(R2);
			SetR3(R3);
			ProgState++;
			break;
		case 4:
			BlankData();
			SetVerbNounAndFlash(16, 63);
// R1 = RTOGO (0.05G to impact), R2 = VIO (V at 0.05G, R3 = TFE (Time till 0.05G)

// calculate range from 0.05G point to landing coords
// calculate lat & long of 0.05G point, same as case 0, but now we'll find the 
//   distance to the LandingLatitude and LandingLongitude variables, which might
//   might have been manually changed.

// estimate 0.05G altitude
			energy = V * V / 2.0 - MUE / rad;
			VPRED = sqrt(2.0 * (energy + MUE / (RE + 122000.0)));
			rentry5 = RE + 86000.0 + 0.94 * (VPRED - 7800.0);

// calculate the current true anomaly and the true anomaly for 0.05G alt
			OurVessel->GetElements(elem,mjd);
			smaj = elem.a;
			ec = elem.e;
			pl = smaj * (1.0 - ec*ec);
			nu = acos((pl / rad - 1.0) / ec);
			nu1 = acos((pl / rentry5 - 1.0) / ec);
			theta = nu - nu1;
			ctheta = cos(theta);
			stheta = sin(theta);
			rx = ctheta;
			ry = 0.0;
			rz = stheta;
			rentry.x = rx * xpx + ry * hx + rz * zpx;
			rentry.y = rx * xpy + ry * hy + rz * zpy;
			rentry.z = rx * xpz + ry * hz + rz * zpz;
			ctheta = 0.9174077;
			stheta = -0.3979486;
			rx=rentry.x;
			ry=rentry.y*ctheta-rentry.z*stheta;
			rz=rentry.z*ctheta+rentry.y*stheta;
			lati = asin(ry);
			longi = atan(rz/rx);
			if(rx < 0.0) longi = 3.14159 + longi;
			theta = oapiGetPlanetCurrentRotation (OurVessel->GetSurfaceRef());
			longi =longi - theta;
			OurVessel->GetEquPos(longitude, latitude, radius);
			manom = acos((1.0 - rad / smaj) / ec);
			manom = manom - ec * sin(manom);
			eanom = acos((1.0 - rentry5 / smaj) / ec);
			eanom = eanom - ec * sin(eanom);
			time = sqrt(smaj * smaj * smaj / MUE) * (manom - eanom);
			theta = 0.00417807 * time; // Earth rotates 0.00417807 degrees per second
			longi = longi - theta / 57.2958;
			while(longi < -3.1415926)longi += 6.2831852;
			while(longi > 3.1415926)longi -= 6.2831852;
			R1 = (int) (-63710.0 * acos(sin(lati) *  sin(0.01745 * LandingLatitude) + cos(lati) * cos(0.01745 * LandingLatitude) * cos(0.01745 * LandingLongitude - longi)));
			if(VPRED < 7925.0) TIMETOGO = 142.7 * R1 / VPRED;
			else TIMETOGO = R1 / 55.5;
			theta = 0.00417807 * TIMETOGO; 
			longi = longi - theta / 57.2958;
			while(longi < -3.1415926)longi += 6.2831852;
			while(longi > 3.1415926)longi -= 6.2831852;
// great circle equation
			RANGETOGO = -6371.0 * acos(sin(lati) *  sin(0.01745 * LandingLatitude) + cos(lati) * cos(0.01745 * LandingLatitude) * cos(0.01745 * LandingLongitude - longi));
			R1= (int)(10.0 * RANGETOGO);
			SetR1(R1);
			energy = V * V / 2.0 - MUE / rad;
			v2 = sqrt(2.0 * (energy + MUE / rentry5));
			R2 = (int) v2;
			SetR2(R2);
			min = (int) (time / 60.0);
			sec = (int) (time - 60 * min);
			if (min > 59) 
			{
				min = 59;
				sec = 59;
			}
			SetR3(- (min * 1000 + sec));
			SetR3Format(TwoSpaceTwoFormat);
			NextEventTime = simt + 1.0;
			break;
		}
	}
}

void CSMcomputer::Prog61Pressed(int R1, int R2, int R3)

{
	double llat,llong;
	switch (ProgState)
	{
	case 1:
		llat = R1 / 100.0;
		llong = R2 / 100.0;
		if((LandingLatitude != llat)||(LandingLongitude != llong)||(R3 != RollCode))
		{
// Should do a check on the changed numbers here to see if they are reasonable
// Right now, the computer will accept whatever you enter
// Another alternative would be an MFD function that helps plan an acceptable landing point
//		based on the entry guidance capabilities
			SetDesiredLanding(llat, llong, 0.0);
			RollCode = R3;
			SetVerbNounAndFlash(6, 61);
			SetR1(R1);
			SetR2(R2);
			SetR3(R3);
		}
		else ProgState++;
		break;
	case 3:
		ProgState++;
		break;
	case 4:
		RunProgram(62);
		break;
	}
}
void CSMcomputer::Prog62(double simt)
{
	int R1, R2, R3;
	switch (ProgState)
	{
	case 0:
		BlankData();
		SetVerbNounAndFlash(50,25);
		R1 = 41;
		SetR1(R1);
		ProgState++;
		break;

	case 2:
		BlankData();
		SetVerbNounAndFlash(6, 61);
// R1 = Lat, R2 = Long, R3 = heads-up (+1) or down (-1)
// Last chance to change
		R1 = (int) (LandingLatitude*100.0);
		R2 = (int) (LandingLongitude*100.0);
		R3 = RollCode;
		SetR1(R1);
		SetR2(R2);
		SetR3(R3);
//		sprintf (oapiDebugString(), "R1 = %d  LandingLatitude = %f R2 = %d LandingLongitude = %f R3 = %d RollCode = %d",R1,LandingLatitude,R2,LandingLongitude,R3,RollCode);
		ProgState++;
		break;
	case 4:
		ProgState++;
		break;
	}
}
void CSMcomputer::Prog62Pressed(int R1, int R2, int R3)

{
	switch (ProgState)
	{
	double llat,llong;
	case 1:
		ProgState++;
		break;
	case 3:
		llat = R1/100.0;
		llong = R2/100.0;
		if((LandingLatitude != llat)||(LandingLongitude != llong)||(R3 != RollCode)){
// Should do a check on the changed numbers here to see if they are reasonable
// Right now, the computer will accept whatever you enterd
// Another alternative would be an MFD function that helps plan an acceptable landing point
//		based on the entry guidance capabilities
			SetDesiredLanding(llat, llong, 0.0);
			RollCode = R3;
			SetVerbNounAndFlash(6, 61);
			SetR1(R1);
			SetR2(R2);
			SetR3(R3);
//			sprintf (oapiDebugString(), "R1 = %d  LandingLatitude = %d R2 = %d LandingLongitude = %d R3 = %d RollCode = %d",R1,(int)(LandingLatitude*100.0),R2,(int)(LandingLongitude),R3,RollCode);
		}
		else
		{
			ACALC = 0;
			aTime = 0.0;
			RunProgram(63);
		}
	break;
	}
}

void CSMcomputer::HoldAttitude(double roll,double pitch,double yaw)
{
	int zone,thruston;
	double pitcherror, sliperror, rollerror,roterror,rotvel,rstep,vstep,bias;
	VECTOR3 AngularVel,AirSpeed,LocalUp,GlobalUp,Rvec;
	double radius,Px,Py,Pz,Vx,Vy,Vz,pmag,UPx,UPy,UPz;
	VESSELSTATUS status;

	OurVessel->GetStatus(status);
	radius = sqrt(status.rpos.x*status.rpos.x+status.rpos.y*status.rpos.y+status.rpos.z*status.rpos.z);
	Rvec.x = status.rpos.x/radius;
	Rvec.y = status.rpos.y/radius;
	Rvec.z = status.rpos.z/radius;
	Px = Rvec.y*status.rvel.z-Rvec.z*status.rvel.y;
	Py = Rvec.z*status.rvel.x-Rvec.x*status.rvel.z;
	Pz = Rvec.x*status.rvel.y-Rvec.y*status.rvel.x;
	pmag = sqrt(Px*Px+Py*Py+Pz*Pz);
	Px = Px/pmag;
	Py = Py/pmag;
	Pz = Pz/pmag;
	Vx = Py*Rvec.z-Pz*Rvec.y;
	Vy = Pz*Rvec.x-Px*Rvec.z;
	Vz = Px*Rvec.y-Px*Rvec.x;
	OurVessel->GetAngularVel(AngularVel);
	LocalUp.x = 0.0;
	LocalUp.y = 1.0;
	LocalUp.z = 0.0;
	OurVessel->GlobalRot(LocalUp,GlobalUp);
	UPx = -(GlobalUp.x*Px+GlobalUp.y*Py+GlobalUp.z*Pz);
	UPy = GlobalUp.x*Rvec.x+GlobalUp.y*Rvec.y+GlobalUp.z*Rvec.z;
	UPz = GlobalUp.x*Vx+GlobalUp.y*Vy+GlobalUp.z*Vz;
	rollerror = roll + atan2(UPx, -UPy); 
	OurVessel->GetShipAirspeedVector(AirSpeed);
	pitcherror = atan2(-AirSpeed.y, -AirSpeed.z) - pitch;
	sliperror = yaw - atan2(-AirSpeed.x, -AirSpeed.z);
	while(pitcherror < -PI) pitcherror += 2.*PI;
	while(pitcherror > PI) pitcherror -= 2.*PI;
	while(rollerror < -PI) rollerror += 2.*PI;
	while(rollerror > PI) rollerror -= 2.*PI;
	while(sliperror < -PI) sliperror += 2.*PI;
	while(sliperror > PI) sliperror -= 2.*PI;
	OurVessel->ActivateNavmode(NAVMODE_KILLROT);
	OurVessel->DeactivateNavmode(NAVMODE_KILLROT);
	for(int i=1;i<9;i++)
	{
		SetOutputChannelBit(05,i,false);
		SetOutputChannelBit(06,i,false);
	}
	for(int j=0;j<3;j++)
	{
		vstep = 0.035;
		rstep = 0.063;
		bias = 0.01;
		switch(j)
		{
		case 0:
			rotvel = AngularVel.x;
			roterror = pitcherror;
			break;
		case 1:
			rotvel = AngularVel.y;
			roterror = sliperror;
			break;
		case 2:
			rotvel = AngularVel.z;
			roterror = rollerror;
			break;
		}
		zone = 0;
		if(rotvel > 2.0*vstep) zone = 1;
		else if (rotvel > vstep) zone = 2;
		else if (rotvel > 0.0) zone = 3;
		else if (rotvel > -vstep) zone = 4;
		else if (rotvel > -2.0*vstep) zone = 5;
		else zone = 6;
		thruston = 0;
		switch(zone)
		{
		case 1:
			if(rotvel > 0.25)
				thruston = 1;
			else if(rotvel > 0.2)
				thruston = 0;
			else
				thruston = -1;
			break;
		case 2:
			if (roterror < (rstep + bias))
				thruston  = -1;
			break;
		case 3:
			if(roterror > (rstep+bias))
				thruston = 1;
			else if(roterror < (-rstep + bias))
				thruston = -1;
			break;
		case 4:
			if(roterror > (rstep-bias))
				thruston = 1;
			else if(roterror < (-rstep - bias))
				thruston = -1;
			break;
		case 5:
			if (roterror > (-rstep-bias))
				thruston = 1;
			break;
		case 6:
			if(rotvel < -0.25)
				thruston = -1;
			else if(rotvel < -0.2)
				thruston = 0;
			else
				thruston = 1;
			break;
		}
		switch(j)
		{
		case 0:
			if(thruston < 0)
			{
				SetOutputChannelBit(05,2,true);
				SetOutputChannelBit(05,4,true);
			}
			else if(thruston > 0)
			{
				SetOutputChannelBit(05,1,true);
				SetOutputChannelBit(05,3,true);
			}
			else
			{
				for(int k=1;k<5;k++)
				{
					SetOutputChannelBit(05,k,false);
				}
			}
			break;
		case 1:
			if(thruston < 0)
			{
				SetOutputChannelBit(05,5,true);
				SetOutputChannelBit(05,7,true);
			}
			else if(thruston > 0)
			{
				SetOutputChannelBit(05,6,true);
				SetOutputChannelBit(05,8,true);
			}
			else
			{
				for(int k=5;k<9;k++)
				{
					SetOutputChannelBit(05,k,false);
				}
			}
			break;
		case 2:
			if(thruston < 0)
			{
				SetOutputChannelBit(06,2,true);
				SetOutputChannelBit(06,4,true);
			}
			else if(thruston > 0)
			{
				SetOutputChannelBit(06,1,true);
				SetOutputChannelBit(06,3,true);
			}
			else
			{
				for(int k=1;k<5;k++)
				{
					SetOutputChannelBit(06,k,false);
				}
			}
			break;
		}
	}
}

void CSMcomputer::Prog63(double simt)
{
	VECTOR3 rentry;
	double radius,dTime,vel;
	double longi,lati,rx,ry,rz;
	double rentry5,smaj,ec,pl,nu,nu1,theta,time,ctheta,stheta,mass1;
	double manom,eanom,range,mjd,energy,hx,hy,hz,xpx,xpy,xpz,zpx,zpy,zpz,hm;
	VESSELSTATUS status;
	ELEMENTS elem;
// R1 = G, R2 = V_inertial, R3 = range to splash

// Calculate D, the drag acceleration (m/sec^2)
	D = OurVessel->GetDrag();
	mass1 = OurVessel->GetMass();
	D = D/mass1;
	ZAcc = D;
	ACALC = 3;
// Every 2 seconds, store the current velocity vector (used to calculate drag in P64)
	dTime = simt - aTime;
	if(dTime >= 2.0)
	{
		OurVessel->GetStatus(status);
		XSPEEDN1 = status.rvel.x;
		YSPEEDN1 = status.rvel.y;
		ZSPEEDN1 = status.rvel.z;
		aTime = simt;
	}
	if(simt > NextEventTime)
	{
		NextEventTime = simt + 0.1;
// Need to calculate TIME5, RTOSPLASH again for possible display
// estimate 0.05G altitude
		OurVessel->GetStatus(status);
		radius = sqrt(status.rpos.x*status.rpos.x+status.rpos.y*status.rpos.y+status.rpos.z*status.rpos.z);
		vel=sqrt(status.rvel.x*status.rvel.x+status.rvel.y*status.rvel.y+status.rvel.z*status.rvel.z);
		V = vel;
		RDOT = (status.rvel.x*status.rpos.x+status.rvel.y*status.rpos.y+status.rvel.z*status.rpos.z)/radius;
		energy = V * V / 2.0 - MUE / radius;
		VPRED = sqrt(2.0 * (energy + MUE / (RE + 122000.0)));
		rentry5 = RE + 86000.0 + 0.94 * (VPRED - 7800.0);
// calculate the current true anomaly and the true anomaly for 0.05G alt
		OurVessel->GetElements(elem,mjd);
		smaj = elem.a;
		ec = elem.e;
		pl = smaj * (1.0 - ec*ec);
		manom = acos((1.0 - radius / smaj) / ec);
		manom = manom - ec * sin(manom);
		eanom = acos((1.0 - rentry5 / smaj) / ec);
		eanom = eanom - ec * sin(eanom);
		time = sqrt(smaj * smaj * smaj / MUE) * (manom - eanom);
		TIME5 = time;
		OurVessel->GetEquPos(longi, lati, radius);
		RTOSPLASH = -6371.0 * acos(sin(lati) *  sin(0.01745 * LandingLatitude) + cos(lati) * cos(0.01745 * LandingLatitude) * cos(0.01745 * LandingLongitude - longi));

		switch (ProgState)
		{
		case 0:
			BlankData();
			SetVerbNoun(16,64);
			NextEventTime = simt + 1.5;
			RELVELSW = FALSE;

			ProgState++;
//Need to calculate VIO, RANGETOGO for possible display, but only once.
			VIO = sqrt(2.0 * (energy + MUE / rentry5));
			ROLLC = 0.0;
// calculate the current true anomaly and the true anomaly for 0.05G alt
			xpx = status.rpos.x / radius;
			xpy = status.rpos.y / radius;
			xpz = status.rpos.z / radius;
// calculate angular momentum vector hx, hy, hz; h = r x v
			hx = -(status.rpos.y * status.rvel.z - status.rpos.z * status.rvel.y);
			hy = -(status.rpos.z * status.rvel.x - status.rpos.x * status.rvel.z);
			hz = -(status.rpos.x * status.rvel.y - status.rpos.y * status.rvel.x);
// normalize
			hm = sqrt(hx * hx + hy * hy + hz * hz);
			hx = hx / hm;
			hy = hy / hm;
			hz = hz / hm;
			zpx= -(hy * xpz - hz * xpy);
			zpy= -(hz * xpx - hx * xpz);
			zpz= -(hx * xpy - hy * xpx);
			OurVessel->GetElements(elem,mjd);
			smaj = elem.a;
			ec = elem.e;
			pl = smaj * (1.0 - ec*ec);
			nu = acos((pl / radius - 1.0) / ec);
			nu1 = acos((pl / rentry5 - 1.0) / ec);
			theta = nu - nu1;
			ctheta = cos(theta);
			stheta = sin(theta);
			rx = ctheta;
			ry = 0.0;
			rz = stheta;
			rentry.x = rx * xpx + ry * hx + rz * zpx;
			rentry.y = rx * xpy + ry * hy + rz * zpy;
			rentry.z = rx * xpz + ry * hz + rz * zpz;
			LATANG =  (rentry.x * hx + rentry.y * hy + rentry.z * hz);

			ctheta = 0.9174077;
			stheta = -0.3979486;
			rx=rentry.x;
			ry=rentry.y*ctheta-rentry.z*stheta;
			rz=rentry.z*ctheta+rentry.y*stheta;
			lati = asin(ry);
			longi = atan(rz/rx);
			if(rx < 0.0) longi = 3.14159 + longi;
			theta = oapiGetPlanetCurrentRotation (OurVessel->GetSurfaceRef());
			longi = longi - theta;
			theta = 0.00417807 * TIME5; // Earth rotates 0.00417807 degrees per second
			longi = longi - theta / 57.2958;
			while(longi < -3.1415926)longi += 6.2831852;
			while(longi > 3.1415926)longi -= 6.2831852;
// calculate range from entry point to splashdown
			range = -6371.0 * acos(sin(lati) *  sin(0.01745 * LandingLatitude) + cos(lati) * cos(0.01745 * LandingLatitude) * cos(0.01745 * LandingLongitude - longi));
			if(VPRED < 7925.0) TIMETOGO = 1427.0 * range / VPRED;
			else TIMETOGO = range / 5.55;
			theta = 0.00417807 * TIMETOGO; 
			longi = longi - theta / 57.2958;
			while(longi < -3.1415926)longi += 6.2831852;
			while(longi > 3.1415926)longi -= 6.2831852;
// great circle equation
			RANGETOGO = -6371.0 * acos(sin(lati) *  sin(0.01745 * LandingLatitude) + cos(lati) * cos(0.01745 * LandingLatitude) * cos(0.01745 * LandingLongitude - longi));
			break;
		case 1:
// waiting to pass 0.05G
			if((ZAcc > 0.49) && (ACALC > 2))
			{
				for(int i=1;i<9;i++)
				{
					SetOutputChannelBit(05,i,false);
					SetOutputChannelBit(06,i,false);
				}
				RunProgram(64);
				break;
			}
			HoldAttitude(0.0,0.466,0.0);
			break;
		}
	}
}

void CSMcomputer::Prog64(double simt)
{
	int R1, R2, R3, subprogram, done, zone;
	VECTOR3 rentry, AirSpeed;
	VECTOR3 AngularVel, LocalUp, GlobalUp, Rvec;
	double Px, Py, Pz, Vx, Vy, Vz, pmag, UPx, UPy, UPz;
	double roll;
	double DVX, DVY, DVZ, agrav, mass, lati, longi;
	double radius, radius2, dTime, rollerror, hx, hy, hz;
	double theta,  rx, ry, rz, ctheta, stheta, xpx, xpy, xpz, rstep, vstep;
	double sliperror, roterror, rotvel;
	double VSQ, LEQ, THETA, THETNM, XD, VD;
	double LoverD1, COSG, TEM1B, frac, rtogo, bias;
	double DVL, GAMMAL1, WT, vX, vY, vZ, mass1;
	double GAMMAL, VBARS, E, ASKEP;
	double ASP1, ASPUP, ASP3, ASPDWN, ASP, DIFF, RDTR, DR, VREFF, RDOTREFF;
	double rdotref, drefr, f2, f1, f3, f4, PREDANGLE, X, Y;
	bool FIVEGSW;
	int thruston, i;
	VESSELSTATUS status;
	OBJHANDLE hPlanet;

	subprogram=0;
	if(simt > NextEventTime)
	{
		dTime = simt-aTime;
		if((dTime > 1.9) && (ProgState > 0))
		{
			OurVessel->GetStatus(status);
			DVX = status.rvel.x - XSPEEDN1;
			DVY = status.rvel.y - YSPEEDN1;
			DVZ = status.rvel.z - ZSPEEDN1;
			hPlanet = OurVessel->GetSurfaceRef();
			mass = oapiGetMass(hPlanet);
			radius2 = status.rpos.x*status.rpos.x+status.rpos.y*status.rpos.y+status.rpos.z*status.rpos.z;
			radius = sqrt(radius2);
			agrav = 6.672e-11 * mass/(radius * radius2);
			DVX = DVX/dTime + agrav * status.rpos.x;
			DVY = DVY/dTime + agrav * status.rpos.y;
			DVZ = DVZ/dTime + agrav * status.rpos.z;
			ZAcc = sqrt(DVX*DVX+DVY*DVY+DVZ*DVZ);
			D = ZAcc;
			radius = sqrt(status.rpos.x*status.rpos.x+status.rpos.y*status.rpos.y+status.rpos.z*status.rpos.z);
			RDOT=(status.rpos.x*status.rvel.x+status.rpos.y*status.rvel.y+status.rpos.z*status.rvel.z)/radius;
			V = sqrt(status.rvel.x*status.rvel.x+status.rvel.y*status.rvel.y+status.rvel.z*status.rvel.z);
			if(!RELVELSW)
			{
				radius = sqrt(status.rpos.x*status.rpos.x+status.rpos.y*status.rpos.y+status.rpos.z*status.rpos.z);
				RDOT = (status.rpos.x*status.rvel.x+status.rpos.y*status.rvel.y+status.rpos.z*status.rvel.z)/radius;
				D = D + 0.5 * dTime * D * (-RDOT / HS - 2.0 * D / V);
			}
			XSPEEDN1 = status.rvel.x;
			YSPEEDN1 = status.rvel.y;
			ZSPEEDN1 = status.rvel.z;
			aTime = simt;
		}
		else
		{
			D = OurVessel->GetDrag();
			mass1 = OurVessel->GetMass();
			D = D/mass1;
			ZAcc = D;
		}
		switch (ProgState)
		{
		case 0:
// Initialize the autopilot
			FIVEGSW = TRUE;
			GONEPAST = FALSE;
			EGSW = FALSE;
			HIND = FALSE;
			LATSW = TRUE;
			RELVELSW = FALSE;
			INRLSW = FALSE;
			GONEBY = FALSE;
			NOSWITCH = FALSE;
			LoverD = - RollCode * LAD;
			if(LATANG < 0.0)K2ROLL = 1;
			else K2ROLL = -1;
			Q7 = Q7F;
			FACTOR = 1.0;
			SELECTOR = 1;
			DIFFOLD = 0.0;
			LEWD = LEWD1;
			DLEWD = DLEWDzero;
			BlankData();
			SetVerbNoun(16,74);
			NextEventTime = simt + 2.0;
			ProgState += 3;
			break;
		case 3:			
			NextEventTime=simt+2.0;
			break;
// Program 65 -> UPCONTROL
		case 4:
			BlankData();
			SetProg(65);
			SetVerbNoun(16,69);
			SELECTOR = 6;
			ProgState++;
			NextEventTime = simt + 1.0;
			break;
		case 5:
			NextEventTime=simt+2.0;
			break;
// Program 66 -> KEP2
		case 6:
			SetProg(66);
			SetVerbNoun(16, 64);
			BlankData();
			SELECTOR = 7;
			ProgState++;
			NextEventTime=simt+1.0;
			break;
		case 7:
			HoldAttitude(0.0,0.466,0.0);
			NextEventTime=simt+2.0;
			break;
// Program 67 -> PREDICT3
		case 8:
			BlankData();
			SetProg(67);
			SetVerbNoun(16,66);
			NextEventTime=simt+1.0;
			SELECTOR = 8;
			ProgState++;
			break;
		case 9:
			SELECTOR = 8;
			NextEventTime=simt+2.0;
			break;
// Program 67.2 ->chute deploy and splashdown
		case 10:
			SELECTOR = 13;
			SetVerbNounAndFlash(16, 67);
			OurVessel->GetEquPos(longi,lati,radius);
			R1 = (int) (63710.0 * acos(sin(lati) *  sin(0.01745 * LandingLatitude) + cos(lati) * cos(0.01745 * LandingLatitude) * cos(0.01745 * LandingLongitude - longi)));
			R2 = (int) (5730.0*lati);
			R3 = (int) (5730.0*longi);
			SetR1(R1);
			SetR2(R2);
			SetR3(R3);
			NextEventTime=simt+2.0;
			break;
		}
// Guidance Section
// --------TARGETING-----------
		ctheta = 0.9174077;
		stheta = 0.3979486;
// xp vector points north (unit vector)
		xpx = 0.0;
		xpy = ctheta;
		xpz = stheta;

		OurVessel->GetStatus(status);
		vX = status.rvel.x;
		vY = status.rvel.y;
		vZ = status.rvel.z;
		radius = sqrt( status.rpos.x * status.rpos.x + status.rpos.y * status.rpos.y + status.rpos.z * status.rpos.z);
		if(RELVELSW)
		{
			vX = vX + 471.43 * ( xpy * status.rpos.z - xpz * status.rpos.y ) / radius;
			vY = vY + 471.43 * ( xpz * status.rpos.x - xpx * status.rpos.z ) / radius;
			vZ = vZ + 471.43 * ( xpx * status.rpos.y - xpy * status.rpos.x ) / radius;
		}
		V = sqrt( vX * vX + vY * vY + vZ * vZ );
		RDOT = ( status.rpos.x * vX+status.rpos.y * vY + status.rpos.z * vZ)/radius;
		VSQ = V * V / (VSAT * VSAT);
		LEQ = ( VSQ - 1.0 ) * GS;
// UNI = (hx, hy, hz) is unit vector in direction of R x V (normal to orbital plane)
		hx = ( status.rpos.y * vZ - status.rpos.z * vY ) / ( V * radius );
		hy = ( status.rpos.z * vX - status.rpos.x * vZ ) / ( V * radius );
		hz = ( status.rpos.x * vY - status.rpos.y * vX ) / ( V * radius );
// calculate landing point unit vector in ecliptic coords
// add the Earth's current rotation
		theta = oapiGetPlanetCurrentRotation (OurVessel->GetSurfaceRef());
		longi = LandingLongitude / 57.3 + theta;
		lati = LandingLatitude / 57.3;
// (rx, ry, rz) is unit vector landing point in equatorial coords
		ry = sin(lati);
		rx = cos(lati) * cos(longi);
		rz = cos(lati) * sin(longi);
// rotate from equatorial to ecliptic coordinates (23.45 degrees)
		rentry.x = rx;
		rentry.y = ry * ctheta - rz * stheta;
		rentry.z = rz * ctheta + ry * stheta;
// Current angle to landing point
		THETA = acos((rentry.x * status.rpos.x + rentry.y * status.rpos.y + rentry.z * status.rpos.z) / radius);
// WT is the angle the Earth is expected to rotate from now to entry
		if(RELVELSW)WT = 0.0;
		else
		{
			D = D + D * 0.5 * ( -RDOT / HS - 2.0 * D / V ) * dTime;
			if(EGSW)
			{
				if((V - VMIN) < 0) RELVELSW = TRUE;
				WT = 0.000072921 * (RE *THETA) / V;
			}
			else
			{
				WT = 0.000072921 * 1000.0 * THETA;
			}
		}
// Compute landing point vector for the time of landing
		longi = longi + WT;
// (rx, ry, rz) is unit vector landing point in equatorial coords
		ry = sin(lati);
		rx = cos(lati) * cos(longi);
		rz = cos(lati) * sin(longi);
// rotate from equatorial to ecliptic coordinates (23.45 degrees)
		rentry.x = rx;
		rentry.y = ry * ctheta - rz * stheta;
		rentry.z = rz * ctheta + ry * stheta;
// Angle to landing point at time of landing
		THETA = acos((rentry.x * status.rpos.x + rentry.y * status.rpos.y + rentry.z * status.rpos.z) / radius);
// Lateral angle
		LATANG =  rentry.x * hx + rentry.y * hy + rentry.z * hz;
		THETNM = THETA * 6371.0;
		RTOSPLASH = THETNM;
		if(ZAcc > 0.49) FIVEGSW = TRUE;
		else FIVEGSW = FALSE;
		subprogram = SELECTOR;
		done = 0;
		while(done == 0)
		{
			switch(subprogram)
			{
			case 1:
// ----------INITROLL-------------
				if(INRLSW == FALSE)
				{
					if(FIVEGSW == FALSE)
					{
						subprogram = 10;
						break;
					}
					INRLSW = TRUE;
					GONEPAST = FALSE;
					KA = (KA1 * pow((LEQ / GS),3) + KA2) * GS;
					if ((KA - 1.5*GS)> 0) KA = 1.5*GS;
					if((V - VFINAL1) < 0.0)
					{
						SELECTOR = 8;
						subprogram = 10;
						ProgState = 8;
						break;
					}
					Dzero = KA3 * LEQ / GS + KA4;
					if((V - VFINAL + K44 * pow((RDOT / V),3)) > 0.0) LoverD = -LAD;
					else LoverD = LAD;
					subprogram = 10;
					break;
				}
				else
				{
					if((RDOT + VRCONTRL) < 0.0)
					{
						if((D - KA) > 0.0)
						{
							subprogram = 4;
							break;
						}
						else
						{
							subprogram = 10;
							break;
						}
					}
					else
					{
						SELECTOR = 2;
						subprogram = 2;
						break;
					}
				}
				break;
			case 2:
// ---------------HUNTEST-------------
				if(RDOT < 0){
					V1 = V + RDOT / LAD;
					A0 = (V1 * V1 / (V * V)) * (D + RDOT * RDOT / (2.0 * C1 * HS * LAD));
					A1 = A0;
				}
				else
				{
					V1 = V + RDOT / LEWD;
					A0 = (V1 * V1 / (V * V)) * (D + RDOT * RDOT / (2.0 * C1 * HS * LEWD));
					A1 = D;
				}
				if(LoverD < 0.0) V1 = V1 - VQUIT;
				ALP = 2.0 * C1 * A0 * HS / (LEWD * V1 * V1);
				FACT1 = V1 / (1.0 - ALP);
				FACT2 = ALP * (ALP - 1.0) / A0;
				VL = FACT1 * (1.0 - sqrt(FACT2 * Q7 + ALP));
				if((VL - VLMIN) < 0.0)
				{
					SELECTOR = 8;
					EGSW = TRUE;
					subprogram = 8;
					ProgState = 8;
					break;
				}
				if((VL - VSAT) > 0.0)
				{
					SELECTOR = 2;
					subprogram = 4;
					break;
				}
				if((V1 - VSAT) > 0.0) VS1 = VSAT;
				else VS1 = V1;
				DVL = VS1 - VL;
				DHOOK = (pow((1.0 - VS1 / FACT1),2) - ALP) / FACT2;
				AHOOK = CHOOK * (DHOOK / Q7 - 1.0) / DVL;
				GAMMAL1 = LEWD * (V1 - VL) / VL;
				GAMMAL = GAMMAL1 - (CH1 * GS * DVL * DVL * (1.0 + AHOOK * DVL))/(DHOOK * VL * VL);
				if(GAMMAL < 0.0)
				{
					VL = VL + GAMMAL * VL / (LEWD - (3.0 * AHOOK * DVL * DVL + 2.0 * DVL) * (CH1 * GS / DHOOK * VL));
					Q7 = (pow((1.0 - VL / FACT1),2) - ALP) / FACT2;
					GAMMAL = 0.0;
				}
				GAMMAL1 = GAMMAL1 * (1.0 - Q19) + Q19 * GAMMAL;
				subprogram = 3;
				break;

			case 3:
// ------------RANGE PREDICTION---------------
				VBARS = VL * VL / (VSAT * VSAT);
				COSG = 1.0 - GAMMAL * GAMMAL / 2.0;
				E = sqrt(1.0 + (VBARS - 2.0) * COSG * COSG * VBARS);
				ASKEP = 2.0 * ATK * asin(VBARS * COSG * GAMMAL / E);
				ASP1 = Q2 + Q3 * VL;
				ASPUP = (ATK / RE)*(HS / GAMMAL1) * log(A0 * VL * VL / (Q7 * V1 * V1));
				ASP3 = Q5 * (Q6 - GAMMAL);
				ASPDWN = -RDOT * V * ATK / (A0 * LAD * RE);
				ASP = ASKEP + ASP1 + ASPUP + ASP3 + ASPDWN;
				DIFF = THETNM - ASP;
				if((fabs(DIFF) - 46.3) < 0.0)
				{
					ProgState++;
					SELECTOR = 6;
					subprogram = 6;
					break;
				}
				if((HIND == FALSE) && (DIFF < 0.0))
				{
					DIFFOLD = DIFF;
					Q7 = Q7F;
					subprogram = 4;
					break;
				}
				DLEWD = DLEWD * DIFF / (DIFFOLD - DIFF);
				if((LEWD + DLEWD) < 0.0) DLEWD = -LEWD / 2.0;
				LEWD = LEWD + DLEWD;
				HIND = TRUE;
				DIFFOLD = DIFF;
				Q7 = Q7F;
				subprogram = 2;
				break;

			case 4:
// --------------CONSTD----------------
				LoverD = -LEQ / Dzero + C16 *( D-Dzero) - C17 * (RDOT + 2.0 * HS * Dzero / V);
		
			case 5:
// -------------NEGTEST---------------
				if((D - C20) > 0.0)
				{
					LATSW = FALSE;
					if(LoverD < 0.0) LoverD = 0.0;
				}
				subprogram = 10;
				break;

			case 6:
// ------------UPCONTRL------------
				if((V - V1) > 0.0)
				{
					RDTR = LAD * (V1 - V);
					DR = V * V * A0 / (V1 * V1) - RDTR * RDTR / (2.0 * C1 * HS * LAD);
					LoverD = LAD + C16 * (D - DR) - C17 * (RDOT - RDTR);
					subprogram = 5;
					break;
				}
				if((D - Q7) < 0.0)
				{
					SELECTOR = 7;
					subprogram = 7;
					ProgState++;
					break;
				}
				if((RDOT < 0.0) && ((V - VL - C18) < 0.0))
				{
					SELECTOR = 8;
					EGSW = TRUE;
					subprogram = 8;
					ProgState = 8;
					break;
				}
				if((A0 - D) < 0.0)
				{
					LoverD = LAD;
					subprogram = 10;
					break;
				}
				VREFF = FACT1 * (1.0 - sqrt(FACT2 * D + ALP));
				if((VREFF - VS1) > 0.0) RDOTREFF = LEWD * (V1 - VREFF);
				else RDOTREFF = LEWD * (V1 - VREFF) - CH1 * GS * pow((VS1 - VREFF),2) * (1.0 + AHOOK * (VS1 - VREFF)) / (DHOOK * VREFF);
				if((D - Q7MIN) > 0.0) 
					FACTOR = (D - Q7) / (A1 - Q7);
				TEM1B = -3.281 * (KB2 * FACTOR * (KB1 * FACTOR * (RDOT - RDOTREFF) + V - VREFF));
				if((fabs(TEM1B) - POINT1) > 0.0)
					TEM1B = (POINT1 + POINT1 * (fabs(TEM1B) - POINT1)) * TEM1B / fabs(TEM1B);
				LoverD = LEWD + TEM1B;
				subprogram = 5;
				break;

			case 7:
// ----------------KEP2----------------
				ROLLC = 0.0;
				if(FIVEGSW == FALSE) ROLLC = 0.0;
				if((D - Q7F - KDMIN) > 0.0)
				{
					ProgState++;
					EGSW = TRUE;
					SELECTOR = 8;
					subprogram = 8;
					break;
				}
				else
				{
					subprogram = 12;
					break;
				}
				break;

			case 8:
// ---------------PREDICT3---------------
				for (i = 0; (i < 13) && (VREF[i+1] < V); i++);
				frac = (V - VREF[i])/(VREF[i+1]-VREF[i]);
				rdotref=(1.0-frac)*RDOTREF[i]+frac*RDOTREF[i+1];
				rtogo=(1.0-frac)*RTOGO[i]+frac*RTOGO[i+1];
				drefr=(1.0-frac)*DREFR[i]+frac*DREFR[i+1];
				f2=(1.0-frac)*F2[i]+frac*F2[i+1];
				f1=(1.0-frac)*F1[i]+frac*F1[i+1];
				f3=(1.0-frac)*F3[i]+frac*F3[i+1];
				f4=(1.0-frac)*F4[i]+frac*F4[i+1];
				PREDANGLE = rtogo + f2*(RDOT-rdotref)+f1*(D-drefr);
				if((OurVessel->GetAirspeed()-VQUIT) < 0.0)
				{
					ProgState++;
					for(i=1;i<9;i++)
					{
						SetOutputChannelBit(05,i,false);
						SetOutputChannelBit(06,i,false);
					}
					done=1;
					break;
				}
				if(GONEPAST == FALSE)
				{
					DNRNGERR = PREDANGLE - ATK*THETA;
					xpx = rentry.y*status.rpos.z-rentry.z*status.rpos.y;
					xpy = rentry.z*status.rpos.x-rentry.x*status.rpos.z;
					xpz = rentry.x*status.rpos.y-rentry.y*status.rpos.x;
					if((xpx*hx+xpy*hy+xpz*hz) < 0.0)
					{
						LoverD = LOD + 4.0*(ATK*THETA-PREDANGLE)/f3;
						subprogram = 9;
						break;
					}
					GONEPAST = TRUE;
					GONEBY = TRUE;
					LoverD = -LAD;
					subprogram = 9;
					break;
				}
				else
				{
					LoverD = -LAD;
					subprogram = 9;
					break;
				}
			case 9:
// ------------GLIMITER--------------
				if((GMAX / 2.0 - D) > 0.0)
				{
					subprogram = 10;
					break;
				}
				if((GMAX - D) < 0.0)
				{
					LoverD = LAD;
					subprogram = 10;
					break;
				}
				X = sqrt(2.0*HS*(GMAX-D)*(LEQ/GMAX+LAD)+pow((2.0*HS*GMAX/V),2));
				if((RDOT + X) < 0.0) LoverD = LAD;
				subprogram = 10;
				break;

			case 10:
// --------------310 (Lateral Control)---------------
				LoverD1 = LoverD;
				XRNGERR = -6371.0 * LATANG;
				if(GONEPAST == FALSE)
				{
					Y = KLAT * VSQ + LATBIAS;
					if((fabs(LoverD) - LDCMINR) > 0.0)
					{
						Y=Y/2.0;
						if((K2ROLL * LATANG) < 0.0)
						{
							LoverD1 = LDCMINR * LoverD / fabs(LoverD);
							if((fabs(LoverD1 / LAD) - 1.0) > 0.0) ROLLC = K2ROLL * acos(LoverD / fabs(LoverD));
							else ROLLC = K2ROLL * acos(LoverD1 / LAD);
				ROLLC = -ROLLC;
							NOSWITCH = FALSE;
							subprogram = 11;
							break;
						}
					}
					if((K2ROLL * LATANG - Y) > 0.0)
					{
						if(NOSWITCH == FALSE) K2ROLL = -K2ROLL;
					}
				}
				if((fabs(LoverD1 / LAD) - 1.0) > 0.0) ROLLC = K2ROLL * acos(LoverD / fabs(LoverD));
				else ROLLC = K2ROLL * acos(LoverD1 / LAD);
				ROLLC = -ROLLC;
				NOSWITCH = FALSE;
				subprogram = 11;
				break;
// command the roll:
			case 11:
			case 12:
			case 13:
					done = 1;
			}
		}
	}
	if(simt > NextControlTime)
	{
		NextControlTime = simt + 0.1;
		OurVessel->ActivateNavmode(NAVMODE_KILLROT);
		OurVessel->DeactivateNavmode(NAVMODE_KILLROT);
		int i=0;
		switch(SELECTOR)
		{
		case 13:
			for(i=1;i<9;i++)
			{
				SetOutputChannelBit(05,i,false);
				SetOutputChannelBit(06,i,false);
			}
			break;
		default:
			OurVessel->GetStatus(status);
			radius = sqrt(status.rpos.x*status.rpos.x+status.rpos.y*status.rpos.y+status.rpos.z*status.rpos.z);
			Rvec.x = status.rpos.x/radius;
			Rvec.y = status.rpos.y/radius;
			Rvec.z = status.rpos.z/radius;
			Px = Rvec.y*status.rvel.z-Rvec.z*status.rvel.y;
			Py = Rvec.z*status.rvel.x-Rvec.x*status.rvel.z;
			Pz = Rvec.x*status.rvel.y-Rvec.y*status.rvel.x;
			pmag = sqrt(Px*Px+Py*Py+Pz*Pz);
			Px = Px/pmag;
			Py = Py/pmag;
			Pz = Pz/pmag;
			Vx = Py*Rvec.z-Pz*Rvec.y;
			Vy = Pz*Rvec.x-Px*Rvec.z;
			Vz = Px*Rvec.y-Px*Rvec.x;
			OurVessel->GetAngularVel(AngularVel);
			LocalUp.x = 0.0;
			LocalUp.y = -1.0;
			LocalUp.z = 0.0;
			OurVessel->GlobalRot(LocalUp,GlobalUp);
			UPx = -(GlobalUp.x*Px+GlobalUp.y*Py+GlobalUp.z*Pz);
			UPy = GlobalUp.x*Rvec.x+GlobalUp.y*Rvec.y+GlobalUp.z*Rvec.z;
			UPz = GlobalUp.x*Vx+GlobalUp.y*Vy+GlobalUp.z*Vz;
//-------ROLL CONTROL--------
//	Modeled after the real DAP equations. 
//	Attempts to roll to commanded roll angle (ROLLC) as quickly as possible.
			if(UPy > 0.0)
				roll = atan(UPx/UPy);
			else
				roll = atan(UPx/UPy) - PI;
			rollerror = -ROLLC - roll;
			OurVessel->GetShipAirspeedVector(AirSpeed);
			sliperror = -atan(AirSpeed.x/AirSpeed.z);
			while(rollerror < -PI) rollerror += 2.*PI;
			while(rollerror > PI) rollerror -= 2.*PI;
			while(sliperror < -PI) sliperror += 2.*PI;
			while(sliperror > PI) sliperror -= 2.*PI;
			OurVessel->ActivateNavmode(NAVMODE_KILLROT);
			OurVessel->DeactivateNavmode(NAVMODE_KILLROT);
			zone =0;
			rotvel = AngularVel.z;
			roterror = rollerror;
			if(rotvel > RMAX) zone = 1;
			else if (rotvel > 0) zone = 2;
			else if (rotvel > -RMAX) zone = 3;
			else zone = 4;
			for(int i = 1;i<9;i++)
			{
					SetOutputChannelBit(05,i,false);
					SetOutputChannelBit(06,i,false);
			}
			switch(zone)
			{
			case 1:
					SetOutputChannelBit(06,2,true);
					SetOutputChannelBit(06,4,true);
				break;
			case 2:
				if(rotvel<0.035)
				{
					if((roterror>(-0.07+2.0*rotvel))&&(roterror<(0.07+2.0*rotvel)))
						break;
				}
				XD = rotvel*rotvel/(2.0*ACONST);
				if(roterror < XD)
				{	
					SetOutputChannelBit(06,2,true);
					SetOutputChannelBit(06,4,true);
				break;
				}
				else
				{
					XD = roterror+XD;
					VD = KCONTROL*(XD - XS);
					if (rotvel<VD) 
					{
						SetOutputChannelBit(06,1,true);
						SetOutputChannelBit(06,3,true);
					}
				break;
				}
			case 3:
				if(rotvel> -0.035)
				{
					if((roterror>(-0.07+2.0*rotvel))&&(roterror<(0.07+2.0*rotvel)))
						break;
				}
				XD = -rotvel*rotvel/(2.0*ACONST);
				if(roterror > XD)
				{	
					SetOutputChannelBit(06,1,true);
					SetOutputChannelBit(06,3,true);
				break;
				}
				else
				{
					XD = -roterror-XD;
					VD = -KCONTROL*(XD - XS);
					if (rotvel>VD) 
					{
						SetOutputChannelBit(06,2,true);
						SetOutputChannelBit(06,4,true);
					}
				break;
				}
			case 4:
				{
					SetOutputChannelBit(06,1,true);
					SetOutputChannelBit(06,3,true);
				}
				break;
			}
//-------PITCH CONTROL--------
// Damps out large pitch rates
			if(AngularVel.x > 0.035)
			{
				SetOutputChannelBit(05,2,true);
				SetOutputChannelBit(05,4,true);
			}
			else if (AngularVel.x < -0.035)
			{
				SetOutputChannelBit(05,1,true);
				SetOutputChannelBit(05,3,true);
			}
//-------YAW CONTROL--------
// Tries to keep yaw (relative to velocity vector) close to zero
			vstep = 0.035;
			rstep = 0.063;
			bias = 0.01;
			rotvel = AngularVel.y;
			roterror = sliperror;
			zone = 0;
			if(rotvel > 2.0*vstep) zone = 1;
			else if (rotvel > vstep) zone = 2;
			else if (rotvel > 0.0) zone = 3;
			else if (rotvel > -vstep) zone = 4;
			else if (rotvel > -2.0*vstep) zone = 5;
			else zone = 6;
			thruston = 0;
			switch(zone)
			{
			case 1:
				if(rotvel > 0.25)
					thruston = 1;
				else if(rotvel > 0.2)
					thruston = 0;
				else
					thruston = -1;
				break;
			case 2:
				if (roterror < (rstep + bias))
					thruston  = -1;
				break;
			case 3:
				if(roterror > (rstep+bias))
					thruston = 1;
				else if(roterror < (-rstep + bias))
					thruston = -1;
				break;
			case 4:
				if(roterror > (rstep-bias))
					thruston = 1;
				else if(roterror < (-rstep - bias))
					thruston = -1;
				break;
			case 5:
				if (roterror > (-rstep-bias))
					thruston = 1;
				break;
			case 6:
				if(rotvel < -0.25)
					thruston = -1;
				else if(rotvel < -0.2)
					thruston = 0;
				else
					thruston = 1;
				break;
			}
			if(thruston < 0)
			{
				SetOutputChannelBit(05,5,true);
				SetOutputChannelBit(05,7,true);
			}
			else if(thruston > 0)
			{
				SetOutputChannelBit(05,6,true);
				SetOutputChannelBit(05,8,true);
			}
			else
			{
				for(int k=5;k<9;k++)
				{
					SetOutputChannelBit(05,k,false);
				}
			}
		}
	}
}

void CSMcomputer::Prog63Pressed(int R1, int R2, int R3)

{
	switch (ProgState)
	{
	case 5:
		SetVerbNoun(16, 74);
		break;
	}
}

//
// Prog 15: TLI burn.
//

void CSMcomputer::DoTLICalcs(double simt)

{
	extern void PredictPosVelVectors(const VECTOR3 &Pos, const VECTOR3 &Vel, double a, double Mu,
						  double Time, VECTOR3 &NewPos, VECTOR3 &NewVel, double &NewVelMag);
	//
	// We know the expected deltaV, so we need to calculate the time for the burn,
	// and the expected end velocity.
	//

	double mass = iu.GetMass();
	double isp = VesselISP;
	double fuelmass = iu.GetFuelMass();
	double thrust = MaxThrust;

	double massrequired = mass * (1.0 - exp(-((DesiredDeltaV - ThrustDecayDV) / isp)));

	LightCompActy();

	if (massrequired > fuelmass) {
		ProgState = 2;
		return;
	}

	double deltaT = massrequired / (thrust / isp);

	//
	// Start the burn early, subtracting half the burn length.
	//

	BurnStartTime = BurnTime - (deltaT / 2.0);
	BurnEndTime = BurnTime + (deltaT / 2.0);

	//
	// Now calculate the cutoff velocity.
	//

	VECTOR3 Pos, Vel;
	OurVessel->GetRelativePos(OurVessel->GetGravityRef(), Pos);
	OurVessel->GetRelativeVel(OurVessel->GetGravityRef(), Vel);

	ELEMENTS el;
	double mjd_ref;
	OurVessel->GetElements(el, mjd_ref);

	VECTOR3 NewPos, NewVel;
	double NewVelMag;

	PredictPosVelVectors(Pos, Vel, el.a, 3.986e14,
						  BurnTime - LastTimestep, NewPos, NewVel, NewVelMag);

	CutOffVel = NewVelMag + DesiredDeltaV - ThrustDecayDV;
}

double CSMcomputer::CurrentG()

{
	OBJHANDLE hbody = OurVessel->GetGravityRef();
	double bradius = oapiGetSize(hbody);

	double CurrentDist = bradius + CurrentAlt;
	return (G * bradius * bradius) / (CurrentDist * CurrentDist);
}

void CSMcomputer::UpdateTLICalcs(double simt)

{
	double MaxE = CutOffVel * CutOffVel;
	double deltaE = 2.0 * (CurrentG() * (CurrentAlt - LastAlt));

	LightCompActy();
	LastAlt = CurrentAlt;
	CutOffVel = sqrt(MaxE - deltaE);
}

void CSMcomputer::Prog59(double simt)

{
	switch (ProgState)
	{
	case 0:
		SetVerbNounAndFlash(06, 39);
		ProgState++;
		break;

	case 100:
	case 200:
	case 300:
		SetVerbNounAndFlash(16, 76);
		SetR1(VesselStatusDisplay);
		ProgState++;
		break;
	}
}

void CSMcomputer::Prog15(double simt)

{
#if 0
	if (ProgState < 17 && ) {
		AbortWithError(021200);
		return;
	}
#endif

	switch (ProgState) {

	//
	// First display time to burn.
	//

	case 0:
		LightCompActy();
		FlagWord2.u.STEERSW = 0;
		SetOutputChannelBit(012, 9, true);
		BurnTime = 0;
		SetVerbNounAndFlash(6, 33);
		ProgState++;
		break;

	case 2:
		LightCompActy();
		SetVerbNounAndFlash(6, 14);
		ProgState++;
		break;

	case 4:
		//
		// Update state.
		//

		iu.GetVesselStats(VesselISP, MaxThrust);

		DoTLICalcs(simt);

		SetVerbNounAndFlash(6, 95);
		ProgState++;

		NextEventTime = (BurnStartTime - (9.0 * 60.0 + 38.0));

		if (NextEventTime < simt)
			NextEventTime = simt;

		break;

	//
	// 9:38 before ignition, signal the SIVb to start.
	//

	case 5:
		if (simt > NextEventTime)
		{
			LightCompActy();
			LightUplink();
			SetOutputChannelBit(012, 13, true);

			ProgState++;
			NextEventTime += 10;
		}
		break;

	case 6:
		if (simt > NextEventTime)
		{
			LightCompActy();
			DoTLICalcs(simt);
			ClearUplink();

			ProgState++;
			NextEventTime = BurnStartTime - 105;
		}
		break;

	case 7:
		if (simt > NextEventTime) {
			BlankAll();

			LightCompActy();

			VerbRunning = 0;
			NounRunning = 0;

			ProgState++;
			NextEventTime += 5;
		}
		break;

	case 8:
		if (simt > NextEventTime) {
			Saturn *sat = (Saturn *) OurVessel;

			UnBlankAll();
			SetVerbNounAndFlash(16, 95);

			DoTLICalcs(simt);

			ProgState++;
			NextEventTime += 10;
		}
		break;

	case 9:
		if (simt >= NextEventTime) 
		{
			DoTLICalcs(simt);

			if (simt > (BurnStartTime - 20)) {
				NextEventTime = simt + 2.0;
			}
			else {
				NextEventTime = BurnStartTime - 84.0;
				ProgState++;
			}
		}
		break;

	case 10:
		if (simt >= NextEventTime) {
			LightCompActy();
			NextEventTime = BurnStartTime - 18;
			ProgState++;
		}
		break;

	case 11:
		if (simt >= NextEventTime) {
			LightCompActy();
			NextEventTime = BurnStartTime - 10.9;
			ProgState++;
		}
		break;

	case 12:
		if (simt >= NextEventTime) {
			DoTLICalcs(simt);

			ProgState++;
			NextEventTime = BurnStartTime - 1;
		}
		break;

	case 13:
		if (simt >= NextEventTime) {
			LightCompActy();

			NextEventTime = BurnStartTime;
			ProgState++;
		}
		break;

	case 14:
		if (simt > BurnStartTime) {
			LightCompActy();
			LastAlt = CurrentAlt;

			NextEventTime = simt + 1;
			ProgState++;
		}
		break;

	//
	// Ignition!

	case 15:
		if (OurVessel->GetEngineLevel(ENGINE_MAIN) >= 1.0) 
		{
			LightCompActy();
			ProgState++;
		}

		//
		// And fall through to normal processing.
		//

	case 16:
		if (simt > NextEventTime)
		{
			UpdateTLICalcs(simt);
			NextEventTime = simt + 0.5;
		}

		//
		// Cut off the engine at the appropriate velocity.
		//
		if (CurrentRVel >= CutOffVel) {
			SetOutputChannelBit(012, 13, false);
			SetOutputChannelBit(012, 14, true);
			SetVerbNoun(6, 95);

			NextEventTime = simt + 2.0;
			ProgState = 17;
		}
		break;

	case 17:
		if (simt >= NextEventTime) {
			UpdateTLICalcs(simt);
			LightCompActy();
			FlagWord5.u.ENGONBIT = 0;
			FlagWord2.u.STEERSW = 0;

			ProgState++;
		}
		break;
	}
}

//
// Data entry for Prog 15.
//

void CSMcomputer::Prog15Pressed(int R1, int R2, int R3)

{
	switch (ProgState) {

	case 1:
		//
		// Waiting for burn start time.
		//

		UpdateBurnTime(R1, R2, R3);

		if ((BurnTime - LastTimestep) < 600.0) {
			RaiseAlarm(0603); // Not the correct alarm, but close enough.
		}
		else
			ProgState++;
		break;

	case 3:
		//
		// Waiting for delta-V.
		//

		if (R1 < 0) {
			LightOprErr();
			return;
		}

		DesiredDeltaV = GetVel((double)R1);

		DoTLICalcs(LastTimestep);

		ProgState++;
		break;

	case 18:
		//
		// Finished.
		//

		NounRunning = 0;
		VerbRunning = 0;

		AwaitProgram();
		BlankData();
		ProgState++;
		break;

	default:
		//
		// Anything else is an error
		//

		LightOprErr();
		break;
	}
}


//
// Data entry for Prog 59.
//

void CSMcomputer::Prog59Pressed(int R1, int R2, int R3)

{
	switch (ProgState)
	{
	case 101:
		if (lv.IsVentable())
		{
			if (!lv.IsVenting())
			{
				lv.StartVenting();
			}
			else
			{
				lv.StopVenting();
			}
			return;
		}
		break;

	default:
		if (R1 > 0 && R1 < 4)
		{
			ProgState = 100 * R1;
			VesselStatusDisplay = R1;
			return;
		}
		break;
	}

	LightOprErr();
}

void CSMcomputer::agcTimestep(double simt, double simdt)
{
	// Do single timesteps to maintain sync with telemetry engine
	SingleTimestepPrep(simt, simdt);        // Setup
	if (LastCycled == 0) {					// Use simdt as difference if new run
		LastCycled = (simt - simdt); 
	}	  
	double ThisTime = LastCycled;			// Save here
	
	long cycles = (long)((simt - LastCycled) / 0.00001171875);	// Get number of CPU cycles to do
	LastCycled += (0.00001171875 * cycles);						// Preserve the remainder
	long x = 0; 
	while(x < cycles) {
		SingleTimestep();
		ThisTime += 0.00001171875;								// Add time
		if((ThisTime - sat->pcm.last_update) > 0.00015625) {	// If a step is needed
			sat->pcm.TimeStep(ThisTime);						// do it
		}
		x++;
	}
}

void CSMcomputer::Run ()
	{
		while(true)
		{
			timeStepEvent.Wait();
			{
				Lock lock(agcCycleMutex);
				agcTimestep(thread_simt,thread_simdt);
			}
		}
	};


void CSMcomputer::Timestep(double simt, double simdt)

{
	// DS20060302 For joystick stuff below
	sat = (Saturn *) OurVessel;

	if (Yaagc){
		//
		// Reduce time acceleration as per configured, not to jump to x100 or x1000 and freeze the simulation
		//
		
		if( sat->maxTimeAcceleration>0 )
		{
			if( oapiGetTimeAcceleration() > (double)sat->maxTimeAcceleration )
				oapiSetTimeAcceleration(sat->maxTimeAcceleration);
		}
		
		//
		// Do nothing if we have no power. (vAGC)
		//
		if (!IsPowered()){
			// HARDWARE MUST RESTART
			if(vagc.Erasable[0][05] != 04000){				
				// Clear flip-flop based registers
				vagc.Erasable[0][00] = 0;     // A
				vagc.Erasable[0][01] = 0;     // L
				vagc.Erasable[0][02] = 0;     // Q
				vagc.Erasable[0][03] = 0;     // EB
				vagc.Erasable[0][04] = 0;     // FB
				vagc.Erasable[0][05] = 04000; // Z
				vagc.Erasable[0][06] = 0;     // BB
				// Clear ISR flag
				vagc.InIsr = 0;
				// Clear interrupt requests
				vagc.InterruptRequests[0] = 0;
				vagc.InterruptRequests[1] = 0;
				vagc.InterruptRequests[2] = 0;
				vagc.InterruptRequests[3] = 0;
				vagc.InterruptRequests[4] = 0;
				vagc.InterruptRequests[5] = 0;
				vagc.InterruptRequests[6] = 0;
				vagc.InterruptRequests[7] = 0;
				vagc.InterruptRequests[8] = 0;
				vagc.InterruptRequests[9] = 0;
				vagc.InterruptRequests[10] = 0;
				// Reset cycle counter and Extracode flags
				vagc.CycleCounter = 0;
				vagc.ExtraCode = 0;
				vagc.ExtraDelay = 0;
				// No idea about the interrupts/pending/etc so we reset those
				vagc.AllowInterrupt = 0;				  
				vagc.PendFlag = 0;
				vagc.PendDelay = 0;
				// Don't disturb erasable core
				// IO channels are flip-flop based and should reset, but that's difficult, so we'll ignore it.
				// Light OSCILLATOR FAILURE and CMC WARNING bits to signify power transient, and be forceful about it
				InputChannel[033] &= 017777;
				vagc.InputChannel[033] &= 017777;				
				OutputChannel[033] &= 017777;				
				vagc.Ch33Switches &= 017777;
				// Also, simulate the operation of the VOLTAGE ALARM and light the RESTART light on the DSKY.
				// This happens externally to the AGC program. See CSM 104 SYS HBK pg 399
				vagc.VoltageAlarm = 1;
				sat->dsky.LightRestart();
				sat->dsky2.LightRestart();
				// Reset last cycling time
				LastCycled = 0;
			}
			// We should issue telemetry though.
			sat->pcm.TimeStep(simt);
			return;
		}

		//
		// Initial startup hack for Yaagc.
		//
		if(!PadLoaded) {

			double latitude, longitude, radius, heading;

			// init pad load
			OurVessel->GetEquPos(longitude, latitude, radius);
			oapiGetHeading(OurVessel->GetHandle(), &heading);

			// set launch pad latitude
			vagc.Erasable[5][2] = ConvertDecimalToAGCOctal(latitude / TWO_PI, true);
			vagc.Erasable[5][3] = ConvertDecimalToAGCOctal(latitude / TWO_PI, false);

			// set launch pad azimuth, the VAGC wants to have the negative angle here
			// otherwise the P11 roll error needle isn't working properly			
			vagc.Erasable[5][0] = ConvertDecimalToAGCOctal((heading - TWO_PI) / TWO_PI, true); 

			// Colossus 249 criterium in SetMissionInfo
			if (ApolloNo < 15 || ApolloNo == 1301) {
			
				// set launch pad longitude
				if (longitude < 0){ longitude += TWO_PI; }
				vagc.Erasable[2][0263] = ConvertDecimalToAGCOctal(longitude / TWO_PI, true);
				vagc.Erasable[2][0264] = ConvertDecimalToAGCOctal(longitude / TWO_PI, false);

				// set launch pad altitude
				//vagc.Erasable[2][0272] = 01;	// 17.7 nmi
				vagc.Erasable[2][0272] = 0;
				vagc.Erasable[2][0273] = (int16_t) (0.5 * OurVessel->GetAltitude());
			
				// z-component of the normalized earth's rotational vector in basic reference coord.
				// x and y are 0313 and 0315 and are defined in the scenario
				vagc.Erasable[3][0317] = 037777;
				vagc.Erasable[3][0320] = 037777;

				// set DAP data to CSM mode 
				vagc.Erasable[AGC_BANK(AGC_DAPDTR1)][AGC_ADDR(AGC_DAPDTR1)] = 011102;
				vagc.Erasable[AGC_BANK(AGC_DAPDTR2)][AGC_ADDR(AGC_DAPDTR2)] = 001111;

				// Synchronize clock with launch time (TEPHEM), only Apollo 7,8 and 11 have proper scenarios
				if (ApolloNo == 7 || ApolloNo == 8 || ApolloNo == 11) {
					double tephem = vagc.Erasable[AGC_BANK(01710)][AGC_ADDR(01710)] + 
									vagc.Erasable[AGC_BANK(01707)][AGC_ADDR(01707)] * pow((double) 2., (double) 14.) +
									vagc.Erasable[AGC_BANK(01706)][AGC_ADDR(01706)] * pow((double) 2., (double) 28.);
					tephem = (tephem / 8640000.) + 40038.;
					double clock = (oapiGetSimMJD() - tephem) * 8640000. * pow((double) 2., (double) -28.);
					vagc.Erasable[AGC_BANK(024)][AGC_ADDR(024)] = ConvertDecimalToAGCOctal(clock, true);
					vagc.Erasable[AGC_BANK(025)][AGC_ADDR(025)] = ConvertDecimalToAGCOctal(clock, false);
				}
			
			} else { // Artemis 072

				// set launch pad longitude
				if (longitude < 0) longitude += TWO_PI;
				vagc.Erasable[2][0135] = ConvertDecimalToAGCOctal(longitude / TWO_PI, true);
				vagc.Erasable[2][0136] = ConvertDecimalToAGCOctal(longitude / TWO_PI, false);

				// set launch pad altitude
				//vagc.Erasable[2][0133] = 01;	// 17.7 nmi
				vagc.Erasable[2][0133] = 0;
				vagc.Erasable[2][0134] = (int16_t) (0.5 * OurVessel->GetAltitude());

				// z-component of the normalized earth's rotational vector in basic reference coord.
				// x and y are 0313 and 0315 and are zero
				vagc.Erasable[3][0315] = 037777;	
				vagc.Erasable[3][0316] = 037777;	

				// set DAP data to CSM mode 
				vagc.Erasable[AGC_BANK(AGC_DAPDTR1)][AGC_ADDR(AGC_DAPDTR1) - 1] = 011102;
				vagc.Erasable[AGC_BANK(AGC_DAPDTR2)][AGC_ADDR(AGC_DAPDTR2) - 1] = 001111;
			}
			PadLoaded = true;
		}

		//
		// If MultiThread is enabled and the simulation is accellerated, the run vAGC in the AGC Thread,
		// otherwise run in main thread. at x1 acceleration, it is better to run vAGC totally synchronized
		//
		if(sat->IsMultiThread && oapiGetTimeAcceleration() > 1.0)
		{
			
			Lock lock(agcCycleMutex);
			thread_simt = simt;
			thread_simdt = simdt;
			timeStepEvent.Raise();
		}
		else
			agcTimestep(simt,simdt);

		//
		// Check nonspherical gravity sources
		//
		if (!OurVessel->NonsphericalGravityEnabled()) {
			sprintf(oapiDebugString(), "*** PLEASE ENABLE NONSPHERICAL GRAVITY SOURCES ***");
		}
		// Done!
		return;
	}

	//
	// Do nothing if we have no power (Simple AGC)
	//
	if (!IsPowered())
		return;

	if (!GenericTimestep(simt, simdt)) {

		switch (ProgRunning) {

		//
		// 00: idle program.
		//

		case 0:
			break;

		//
		// 01: pre-launch IMU align.
		//

		case 1:
			Prog01(simt);
			break;

		//
		// 02: pre-launch setup.
		//

		case 2:
			Prog02(simt);
			break;

		//
		// 06: power-down.
		//

		case 6:
			Prog06(simt);
			break;

		//
		// 11: initial launch program.
		//

		case 11:
			Prog11(simt);
			break;

		//
		// 15: TLI program.
		//

		case 15:
			Prog15(simt);
			break;

		//
		// 59: Docking program.
		//

		case 59:
			Prog59(simt);
			break;

		case 61:
			Prog61(simt);
			break;
		case 62:
			Prog62(simt);
			break;
		case 63:
			Prog63(simt);
			break;
		case 64:
			Prog64(simt);
			break;

		}
	}
}

//
// We've been told to terminate the program.
//

void CSMcomputer::TerminateProgram()

{
	//
	// Special-case handling of individual program events.
	//

	switch (ProgRunning) {

	//
	// 11: disable autopilot.
	//

	case 11:
		{
			Saturn *sat = (Saturn *) OurVessel;
			sat->SetAutopilot(false);
		}
		break;

	default:
		TerminateCommonProgram();
		break;

	}

	//
	// In general, just switch to idle and wait for a
	// new program.
	//

	VerbRunning = 0;
	NounRunning = 0;

	BlankData();
	RunProgram(0);
	AwaitProgram();
}

//
// We've been told to proceed with no data. Check where we are
// in the program and proceed apropriately.
//

void CSMcomputer::ProceedNoData()

{
	if (CommonProceedNoData())
		return;

	switch (ProgRunning) {

	case 2:
		if (ProgState == 1 || ProgState == 3) {
			ProgState++;
			return;
		}
		break;

	case 15:
		if (ProgState == 1 || ProgState == 3) {
			ProgState++;
			return;
		}
		break;
	}

	LightOprErr();
}

//
// The prog button has been pressed, so process it.
//

void CSMcomputer::ProgPressed(int R1, int R2, int R3)

{
	if (GenericProgPressed(R1, R2, R3))
		return;

	switch(ProgRunning) {

	case 02:
		Prog02Pressed(R1, R2, R3);
		return;

	case 15:
		Prog15Pressed(R1, R2, R3);
		return;

	case 59:
		Prog59Pressed(R1, R2, R3);
		return;
	case 61:
		Prog61Pressed(R1, R2, R3);
		return;
	case 62:
		Prog62Pressed(R1, R2, R3);
		return;
	case 63:
		Prog63Pressed(R1, R2, R3);
		return;
	}

	LightOprErr();
}

//
// Program flag routines.
//

unsigned int CSMcomputer::GetFlagWord(int num)

{
	switch (num) {
	case 0:
		FlagWord0.u.FREEFLAG = ProgFlag01;
		FlagWord0.u.UNUSED = ProgFlag02;
		return FlagWord0.word;

	case 1:
		return FlagWord1.word;

	case 2:
		return FlagWord2.word;

	case 3:
		return FlagWord3.word;

	case 4:
		return FlagWord4.word;

	case 5:
		FlagWord5.u.RETROFLG = RetroFlag;
		FlagWord5.u.UNUSED1 = ProgFlag03;
		FlagWord5.u.UNUSED2 = ProgFlag04;
		FlagWord5.u.ENGONBIT = BurnFlag;
		return FlagWord5.word;
	}

	//
	// Invalid flag word number.
	//

	return 0;
}

void CSMcomputer::SetFlagWord(int num, unsigned int val)

{
	switch (num) {
	case 0:
		FlagWord0.word = val;
		ProgFlag01 = FlagWord0.u.FREEFLAG;
		ProgFlag02 = FlagWord0.u.UNUSED;
		break;

	case 1:
		FlagWord1.word = val;
		break;

	case 2:
		FlagWord2.word = val;
		break;

	case 3:
		FlagWord3.word = val;
		break;

	case 4:
		FlagWord4.word = val;
		break;

	case 5:
		FlagWord5.word = val;
		RetroFlag = FlagWord5.u.RETROFLG;
		BurnFlag = FlagWord5.u.ENGONBIT;
		ProgFlag03 = FlagWord5.u.UNUSED1;
		ProgFlag04 = FlagWord5.u.UNUSED2;
		break;
	}

	//
	// Invalid flag word number.
	//
}

//
// Access simulated erasable memory.
//

bool CSMcomputer::ReadMemory(unsigned int loc, int &val)

{
	if (!Yaagc) {
		//
		// Note that these values are in OCTAL, so need the
		// zero prefix.
		//

		if (loc >= 074 && loc <= 0107) {
			val = (int) GetFlagWord(loc - 074);
			return true;
		}

		switch (loc)
		{
		case 0110:
			val = (int) (LandingLatitude * 1000.0);
			return true;

		case 0111:
			val = (int) (LandingLongitude * 1000.0);
			return true;

		case 0112:
			val = (int) LandingAltitude;
			return true;

		case 0113:
			val = (int)(LandingLatitude * 100000000.0 - ((int)(LandingLatitude * 1000.0)) * 100000.0);
			return true;

		case 0114:
			val = (int)(LandingLongitude * 100000000.0 - ((int)(LandingLongitude * 1000.0)) * 100000.0);
			return true;

		case 0115:
			val = VesselStatusDisplay;
			return true;

		case 0116:
			val = (GONEPAST ? 1 : 0);
			return true;

		case 0117:
			val = (EGSW ? 1 : 0);
			return true;

		case 0120:
			val = (HIND ? 1 : 0);
			return true;

		case 0121:
			val = (LATSW ? 1 : 0);
			return true;

		case 0122:
			val = (RELVELSW ? 1 : 0);
			return true;

		case 0123:
			val = (INRLSW ? 1 : 0);
			return true;

		case 0124:
			val = (GONEBY ? 1 : 0);
			return true;

		case 0125:
			val = (NOSWITCH ? 1 : 0);
			return true;

		case 0126:
			val = ACALC;
			return true;

		case 0127:
			val = RollCode;
			return true;

		case 0130:
			val = K2ROLL;
			return true;

		case 0131:
			val = SELECTOR;
			return true;

		/// \todo Check scaling of all the doubles

		case 0132:
			val = (int) (KA * 100.0);
			return true;

		case 0133:
			val = (int) (Dzero * 100.0);
			return true;

		case 0134:
			val = (int) (LoverD * 100.0);
			return true;

		case 0135:
			val = (int) (Q7 * 100.0);
			return true;

		case 0136:
			val = (int) (FACTOR * 100.0);
			return true;

		case 0137:
			val = (int) (FACT1 * 100.0);
			return true;

		case 0140:
			val = (int) (FACT2 * 100.0);
			return true;

		case 0141:
			val = (int) (DIFFOLD * 100.0);
			return true;

		case 0142:
			val = (int) (LEWD * 100.0);
			return true;

		case 0143:
			val = (int) (DLEWD * 100.0);
			return true;

		case 0144:
			val = (int) (A0 * 100.0);
			return true;

		case 0145:
			val = (int) (A1 * 100.0);
			return true;

		case 0146:
			val = (int) (VS1 * 100.0);
			return true;

		case 0147:
			val = (int) (ALP * 100.0);
			return true;

		case 0150:
			val = (int) (AHOOK * 100.0);
			return true;

		case 0151:
			val = (int) (DHOOK * 100.0);
			return true;

		case 0152:
			val = (int) (V1 * 100.0);
			return true;

		case 0153:
			val = (int) (V * 100.0);
			return true;

		case 0154:
			val = (int) (D * 100.0);
			return true;

		case 0155:
			val = (int) (PREDGMAX * 100.0);
			return true;

		case 0156:
			val = (int) (VPRED * 100.0);
			return true;

		case 0157:
			val = (int) (GAMMAEI * 100.0);
			return true;

		case 0160:
			val = (int) (TIME5 * 100.0);
			return true;

		case 0161:
			val = (int) (RANGETOGO * 100.0);
			return true;

		case 0162:
			val = (int) (RTOSPLASH * 100.0);
			return true;

		case 0163:
			val = (int) (TIMETOGO * 100.0);
			return true;

		case 0164:
			val = (int) (VIO * 100.0);
			return true;

		case 0165:
			val = (int) (ROLLC * 100.0);
			return true;

		case 0166:
			val = (int) (LATANG * 100.0);
			return true;

		case 0167:
			val = (int) (XRNGERR * 100.0);
			return true;

		case 0170:
			val = (int) (DNRNGERR * 100.0);
			return true;

		case 0171:
			val = (int) (RDOT * 100.0);
			return true;

		case 0172:
			val = (int) (VL * 100.0);
			return true;

		default:
			break;
		}
	}

	return GenericReadMemory(loc, val);
}

void CSMcomputer::WriteMemory(unsigned int loc, int val)

{
	if (!Yaagc) {
		//
		// Note that these values are in OCTAL, so need the
		// zero prefix.
		//

		if (loc >= 074 && loc <= 0107) {
			SetFlagWord(loc - 074, (unsigned int) val);
			return;
		}

		switch (loc)
		{
		case 0110:
			LandingLatitude = ((double) val) / 1000.0;
			break;

		case 0111:
			LandingLongitude = ((double) val) / 1000.0;
			break;

		case 0112:
			LandingAltitude = (double) val;
			break;

		case 0113:
			LandingLatitude = LandingLatitude+((double) val) / 100000000.0;
			break;

		case 0114:
			LandingLongitude = LandingLongitude+((double) val) / 100000000.0;
			break;

		case 0115:
			VesselStatusDisplay = val;
			break;

		case 0116:
			GONEPAST = (val == 1);
			break;

		case 0117:
			EGSW = (val == 1);
			break;

		case 0120:
			HIND = (val == 1);
			break;

		case 0121:
			LATSW = (val == 1);
			break;

		case 0122:
			RELVELSW = (val == 1);
			break;

		case 0123:
			INRLSW = (val == 1);
			break;

		case 0124:
			GONEBY = (val == 1);
			break;

		case 0125:
			NOSWITCH = (val == 1);
			break;

		case 0126:
			ACALC = val;
			break;

		case 0127:
			RollCode = val;
			break;

		case 0130:
			K2ROLL = val;
			break;

		case 0131:
			SELECTOR = val;
			break;

		/// \todo Check scaling of all the doubles

		case 0132:
			KA = ((double) val) / 100.0;
			break;

		case 0133:
			Dzero = ((double) val) / 100.0;
			break;

		case 0134:
			LoverD = ((double) val) / 100.0;
			break;

		case 0135:
			Q7 = ((double) val) / 100.0;
			break;

		case 0136:
			FACTOR = ((double) val) / 100.0;
			break;

		case 0137:
			FACT1 = ((double) val) / 100.0;
			break;

		case 0140:
			FACT2 = ((double) val) / 100.0;
			break;

		case 0141:
			DIFFOLD = ((double) val) / 100.0;
			break;

		case 0142:
			LEWD = ((double) val) / 100.0;
			break;

		case 0143:
			DLEWD = ((double) val) / 100.0;
			break;

		case 0144:
			A0 = ((double) val) / 100.0;
			break;

		case 0145:
			A1 = ((double) val) / 100.0;
			break;

		case 0146:
			VS1 = ((double) val) / 100.0;
			break;

		case 0147:
			ALP = ((double) val) / 100.0;
			break;

		case 0150:
			AHOOK = ((double) val) / 100.0;
			break;

		case 0151:
			DHOOK = ((double) val) / 100.0;
			break;

		case 0152:
			V1 = ((double) val) / 100.0;
			break;

		case 0153:
			V = ((double) val) / 100.0;
			break;

		case 0154:
			D = ((double) val) / 100.0;
			break;

		case 0155:
			PREDGMAX = ((double) val) / 100.0;
			break;

		case 0156:
			VPRED = ((double) val) / 100.0;
			break;

		case 0157:
			GAMMAEI = ((double) val) / 100.0;
			break;

		case 0160:
			TIME5 = ((double) val) / 100.0;
			break;

		case 0161:
			RANGETOGO = ((double) val) / 100.0;
			break;

		case 0162:
			RTOSPLASH = ((double) val) / 100.0;
			break;

		case 0163:
			TIMETOGO = ((double) val) / 100.0;
			break;

		case 0164:
			VIO = ((double) val) / 100.0;
			break;

		case 0165:
			ROLLC = ((double) val) / 100.0;
			break;

		case 0166:
			LATANG = ((double) val) / 100.0;
			break;

		case 0167:
			XRNGERR = ((double) val) / 100.0;
			break;

		case 0170:
			DNRNGERR = ((double) val) / 100.0;
			break;

		case 0171:
			RDOT = ((double) val) / 100.0;
			break;

		case 0172:
			VL = ((double) val) / 100.0;
			break;

		default:
			GenericWriteMemory(loc, val);
			break;
		}
	}
	else {
		GenericWriteMemory(loc, val);
	}
}

///
/// Memory bank checksums for the CSM AGC running Colossus software. These are used to simulate
/// the bank checksum display program in the C++ AGC.
///
/// Checksum values should either be equal to the bank number or negative bank number. Any other
/// value shows a memory error.
///
/// \brief Bank checksums.
///
static unsigned int BankSums[] =
{
	077777, 063743,
	000001, 074057,
	000002, 050032,
	000003, 060757,
	000004, 077375,
	000005, 043705,
	077771, 005143,
	000007, 063337,
	000010, 050763,
	000011, 045337,
	000012, 063341,
	000013, 074774,
	000014, 056404,
	000015, 052523,
	000016, 042460,
	000017, 042700,
	000020, 077316,
	000021, 061425,
	000022, 075664,
	077754, 016525,
	000024, 072000,
	000025, 060227,
	000026, 053053,
	000027, 066445,
	000030, 072005,
	000031, 040224,
	000032, 060374,
	000033, 071456,
	000034, 077620,
	000035, 054100,
	000036, 040555,
	077740, 011751,
	000040, 057252,
	000041, 064667,
	000042, 042661,
	077734, 000152,
};

void CSMcomputer::DisplayBankSum()

{
	int	R1, R3;

	if (BankSumNum > 043)
		BankSumNum = 0;

	R1 = BankSums[BankSumNum * 2];
	R3 = BankSums[(BankSumNum * 2) + 1];

	SetR1Octal(R1);
	SetR2Octal(BankSumNum);
	SetR3Octal(R3);
}

//
// Exernal event handling.
//

void CSMcomputer::Liftoff(double simt)

{

	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	//
	// Ensure autopilot is enabled.
	//

	SetOutputChannelBit(012, 9, false);
	Saturn *Sat = (Saturn *)OurVessel;
	Sat->SetAutopilot(true);

	//
	// If we're not running the Virtual AGC, then switch to program 11.
	//

	if (!Yaagc) {

		//
		// As a quick hack we drive the IMU to prelaunch orientation 
		//

		double heading;
		oapiGetHeading(OurVessel->GetHandle(), &heading);
		imu.DriveGimbals(TWO_PI - heading + (DesiredAzimuth / DEG), 90.0 / DEG, 0.0);

		//
		// Also, we need to reset the AGC time to zero at this point, as the real AGC did.
		//

		TIME1 = 0;

		if (OnStandby())
			Startup();

		FlagWord1.u.NODOP01 = 1;

		RunProgram(11);

		if (!KBCheck()) {
			UnBlankAll();
		}
	} 
}

void CSMcomputer::SetInputChannelBit(int channel, int bit, bool val)

{
	ApolloGuidance::SetInputChannelBit(channel, bit, val);

	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	switch (channel)
	{
	case 030:
		if (bit == 5 && val) {
			Liftoff(CurrentTimestep);	// Liftoff signal
			break;
		}
		else if (!Yaagc && (bit == 11 || bit == 9)) {
			ChannelValue30 val30;

			val30.Value = GetInputChannel(030);
			dsky.SetNoAtt(!val30.Bits.IMUOperate || val30.Bits.IMUCage);
		}
		break;
	}
}

void CSMcomputer::SetOutputChannelBit(int channel, int bit, bool val)

{
	ApolloGuidance::SetOutputChannelBit(channel, bit, val);

	//
	// Special-case processing.
	//

	switch (channel)
	{
	case 012:
		iu.ChannelOutput(channel, OutputChannel[channel]);
		break;
	}
}

void CSMcomputer::SetOutputChannel(int channel, unsigned int val)

{
	ApolloGuidance::SetOutputChannel(channel, val);

	//
	// Special-case processing.
	//

	switch (channel)
	{
	case 012:
		iu.ChannelOutput(channel, val);
		break;
	}
}

//
// We need to pass these I/O channels to both DSKYs.
//

void CSMcomputer::ProcessChannel10(int val)

{
	dsky.ProcessChannel10(val);
	dsky2.ProcessChannel10(val);

	// Gimbal Lock & Prog alarm
	ChannelValue10 val10;
	val10.Value = val;
	if (val10.Bits.a == 12) {
		// Gimbal Lock
		GimbalLockAlarm = ((val10.Value & (1 << 5)) != 0);
		// Prog alarm
		ProgAlarm = ((val10.Value & (1 << 8)) != 0);
	}
}

void CSMcomputer::ProcessChannel11Bit(int bit, bool val)

{
	dsky.ProcessChannel11Bit(bit, val);
	dsky2.ProcessChannel11Bit(bit, val);

	LastOut11 = GetOutputChannel(011);
}

void CSMcomputer::ProcessChannel11(int val)

{
	dsky.ProcessChannel11(val);
	dsky2.ProcessChannel11(val);

	LastOut11 = val;
}

void CSMcomputer::BurnMainEngine(double thrust)

{
	// This is the same the real VAGC does
	if (thrust)
		SetOutputChannelBit(011, 13, 1);
	else
		SetOutputChannelBit(011, 13, 0);

	ApolloGuidance::BurnMainEngine(thrust);
}

//
// Process RCS channels
//

void CSMcomputer::ProcessChannel5(int val)

{
	ChannelValue30 val30;
	val30.Value = GetInputChannel(030);

	Saturn *sat = (Saturn *) OurVessel;
	if (sat->SCContSwitch.IsDown() || sat->THCRotary.IsClockwise()) {
		return;
	}

	CSMOut5 Current;
	CSMOut5 Changed;

	//
	// Get the current state and a mask of any changed state.
	//

	Current.word = val;
	Changed.word = (val ^ LastOut5);

	//
	// Update any thrusters that have changed.
	//

	if (Changed.u.SMA3) {
		sat->rjec.SetThruster(3,Current.u.SMA3 != 0);
	}
	if (Changed.u.SMA4) {
		sat->rjec.SetThruster(2,Current.u.SMA4 != 0);
	}

	if (Changed.u.SMB3) {
		sat->rjec.SetThruster(7,Current.u.SMB3 != 0);
	}
	if (Changed.u.SMB4) {
		sat->rjec.SetThruster(6,Current.u.SMB4 != 0);
	}

	if (Changed.u.SMC3) {
		sat->rjec.SetThruster(1,Current.u.SMC3 != 0);
	}
	if (Changed.u.SMC4) {
		sat->rjec.SetThruster(4,Current.u.SMC4 != 0);
	}

	if (Changed.u.SMD3) {
		sat->rjec.SetThruster(5,Current.u.SMD3 != 0);
	}
	if (Changed.u.SMD4) {
		sat->rjec.SetThruster(8,Current.u.SMD4 != 0);
	}

	LastOut5 = val;
}

void CSMcomputer::ProcessChannel6(int val)

{
	ChannelValue30 val30;
	val30.Value = GetInputChannel(030);

	Saturn *sat = (Saturn *) OurVessel;
	if (sat->SCContSwitch.IsDown() || sat->THCRotary.IsClockwise()) {
		return;
	}

	CSMOut6 Current;
	CSMOut6 Changed;

	//
	// Get the current state and a mask of any changed state.
	//

	Current.word = val;
	Changed.word = (val ^ LastOut6);

	//
	// Update any thrusters that have changed.
	//

	if (Changed.u.SMA1) {
		sat->rjec.SetThruster(13,Current.u.SMA1 != 0);
	}
	if (Changed.u.SMA2) {
		sat->rjec.SetThruster(14,Current.u.SMA2 != 0);
	}

	if (Changed.u.SMB1) {
		sat->rjec.SetThruster(9,Current.u.SMB1 != 0);
	}
	if (Changed.u.SMB2) {
		sat->rjec.SetThruster(12,Current.u.SMB2 != 0);
	}

	if (Changed.u.SMC1) {
		sat->rjec.SetThruster(15,Current.u.SMC1 != 0);
	}
	if (Changed.u.SMC2) {
		sat->rjec.SetThruster(16,Current.u.SMC2 != 0);
	}

	if (Changed.u.SMD1) {
		sat->rjec.SetThruster(11,Current.u.SMD1 != 0);
	}
	if (Changed.u.SMD2) {
		sat->rjec.SetThruster(10,Current.u.SMD2 != 0);
	}

	LastOut6 = val;
}

// DS20060308 FDAI
void CSMcomputer::ProcessIMUCDUErrorCount(int channel, unsigned int val){
	// These pulses work like the TVC pulses.
	// FULL NEEDLE DEFLECTION is 16.88 DEGREES
	// 030 PULSES = MAX IN ONE RELAY EVENT
	// 22 PULSES IS ONE DEGREE, 384 PULSES = FULL SCALE
	// 0.10677083 PIXELS PER PULSE

	Saturn *sat = (Saturn *) OurVessel;
	ChannelValue12 val12;
	if(channel != 012){ val12.Value = GetOutputChannel(012); }else{ val12.Value = val; }
	// 174 = X, 175 = Y, 176 = Z
	if(val12.Bits.CoarseAlignEnable){ return; } // Does not apply to us here.
	switch(channel){
	case 012:
		// Reset FDAI
		if (val12.Bits.EnableIMUCDUErrorCounters) {
			if (sat->gdc.fdai_err_ena == 0) {
				// sprintf(oapiDebugString(),"FDAI: RESET");						
				sat->gdc.fdai_err_x = 0;
				sat->gdc.fdai_err_y = 0;
				sat->gdc.fdai_err_z = 0;
				sat->gdc.fdai_err_ena = 1;
			}
		} else {
			if (sat->gdc.fdai_err_ena == 1) {
				// sprintf(oapiDebugString(),"FDAI: RESET");
				sat->gdc.fdai_err_x = 0;
				sat->gdc.fdai_err_y = 0;
				sat->gdc.fdai_err_z = 0;
			}
			sat->gdc.fdai_err_ena = 0;
		}

		// Reset TVC
		if (val12.Bits.TVCEnable) {
			if (val12.Bits.EnableOpticsCDUErrorCounters) {
				if (!sat->SPSEngine.cmcErrorCountersEnabled) {
					sat->SPSEngine.pitchGimbalActuator.ZeroCMCPosition();
					sat->SPSEngine.yawGimbalActuator.ZeroCMCPosition();
					sat->SPSEngine.cmcErrorCountersEnabled = true;
				}
			} else {
				sat->SPSEngine.cmcErrorCountersEnabled = false;
			}
		}

		// If OPTICS TRACKER switch is not up
		if(sat->ControllerTrackerSwitch.GetState() != THREEPOSSWITCH_UP){
			// leave
			break;
		}else{
			// If the switch is up, print optics register status
			bool IssueDebug = FALSE;
			char DebugMsg[256];

			sprintf(DebugMsg,"OPTICS: ");
			if(val12.Bits.DisengageOpticsDAC){
				IssueDebug = TRUE;
				sprintf(DebugMsg,"%s DISENGAGE-DAC",DebugMsg);
			}
			if(val12.Bits.EnableOpticsCDUErrorCounters){
				IssueDebug = TRUE;
				sprintf(DebugMsg,"%s ENABLE-ERR-CTR",DebugMsg);
			}else{
				// This caused problems.
				// sat->agc.vagc.block_ocdu_err_ctr = 0;
			}
			if(val12.Bits.ZeroOptics){
				IssueDebug = TRUE;
				sprintf(DebugMsg,"%s ZERO-OPTICS",DebugMsg);
			}
			if(val12.Bits.ZeroOpticsCDUs){
				IssueDebug = TRUE;
				sprintf(DebugMsg,"%s ZERO-CDU",DebugMsg);
			}
			if(IssueDebug != FALSE){
				sprintf(oapiDebugString(),"%s",DebugMsg);
			}else{
				sprintf(oapiDebugString(),"OPTICS: NIL");
			}		
		}
		break;
		
	case 0174: // FDAI ROLL ERROR
		if(val12.Bits.EnableIMUCDUErrorCounters){
			int delta = val&0777;
			// Direction for these is inverted.
			if(val&040000){
				sat->gdc.fdai_err_x += delta;
			}else{
				sat->gdc.fdai_err_x -= delta;
			}
		}
//		sprintf(oapiDebugString(),"FDAI: NEEDLES: %d %d %d",sat->gdc.fdai_err_x,sat->gdc.fdai_err_y,sat->gdc.fdai_err_z);
		break;
	
	case 0175: // FDAI PITCH ERROR
		if(val12.Bits.EnableIMUCDUErrorCounters){
			int delta = val&0777;
			if(val&040000){
				sat->gdc.fdai_err_y -= delta;
			}else{
				sat->gdc.fdai_err_y += delta;
			}
		}
//		sprintf(oapiDebugString(),"FDAI: NEEDLES: %d %d %d",sat->gdc.fdai_err_x,sat->gdc.fdai_err_y,sat->gdc.fdai_err_z);
		break;

	case 0176: // FDAI YAW ERROR
		if(val12.Bits.EnableIMUCDUErrorCounters){
			int delta = val&0777;
			if(val&040000){
				sat->gdc.fdai_err_z += delta;
			}else{
				sat->gdc.fdai_err_z -= delta;
			}
		}
//		sprintf(oapiDebugString(),"FDAI: NEEDLES: %d %d %d",sat->gdc.fdai_err_x,sat->gdc.fdai_err_y,sat->gdc.fdai_err_z);
		break;
	}
}

// TVC / Optics control

void CSMcomputer::ProcessChannel160(int val) {
	
	ChannelValue12 val12;
	val12.Value = GetOutputChannel(012);
	Saturn *sat = (Saturn *) OurVessel;
	double error = 0;
	
	// TVC enable controls SPS gimballing.			
	if (val12.Bits.TVCEnable) {
		// TVC PITCH
		int tvc_pitch_pulses = 0;
		double tvc_pitch_cmd = 0;
		// One pulse means .023725 degree of rotation.
		if(val&077000){ // Negative
			tvc_pitch_pulses = (~val)&0777;
			if(tvc_pitch_pulses == 0){ return; } // HACK
			tvc_pitch_cmd = (double)0.023725 * tvc_pitch_pulses;
			tvc_pitch_cmd = 0 - tvc_pitch_cmd; // Invert
		}else{
			tvc_pitch_pulses = val&0777;
			tvc_pitch_cmd = (double)0.023725 * tvc_pitch_pulses;
		}		
		sat->SPSEngine.pitchGimbalActuator.ChangeCMCPosition(tvc_pitch_cmd);

	} else {
		sat->optics.CMCShaftDrive(val,val12.Value);
	}	
}

void CSMcomputer::ProcessChannel161(int val) {

	ChannelValue12 val12;
	val12.Value = GetOutputChannel(012);
	Saturn *sat = (Saturn *) OurVessel;
	double error = 0;
	
	if (val12.Bits.TVCEnable) {
		// TVC YAW
		int tvc_yaw_pulses = 0;
		double tvc_yaw_cmd = 0;		
		if(val&077000){ 
			tvc_yaw_pulses = (~val)&0777;
			if(tvc_yaw_pulses == 0){ return; } 
			tvc_yaw_cmd = (double)0.023725 * tvc_yaw_pulses;
			tvc_yaw_cmd = 0 - tvc_yaw_cmd; 
		}else{
			tvc_yaw_pulses = val&0777;
			tvc_yaw_cmd = (double)0.023725 * tvc_yaw_pulses;
		}				
		sat->SPSEngine.yawGimbalActuator.ChangeCMCPosition(tvc_yaw_cmd);

	} else {
		sat->optics.CMCTrunionDrive(val,val12.Value);
	}	
}

void CSMcomputer::ProcessChannel14(int val){
	// This entire deal is no longer necessary, but we'll leave the stub here in case it's needed later.
	/*
	ChannelValue12 val12;
	ChannelValue14 val14;
	val12.Value = GetOutputChannel(012);
	val14.Value = val;
	Saturn *sat = (Saturn *) OurVessel;	

	if(val12.Bits.TVCEnable){
		return; // Ignore
	} */
}

/// \todo Dirty Hack for the AGC++ attitude control, 
/// remove this and use I/O channels and pulsed thrusters 
/// identical to the VAGC instead

void CSMcomputer::SetAttitudeRotLevel(VECTOR3 level) {
	
	Saturn *sat = (Saturn *) OurVessel;
	if (sat->SCContSwitch.IsUp() && !sat->THCRotary.IsClockwise()) {
		// Ensure RJ/EC power, Auto RCS is not checked currently
		if (sat->SIGCondDriverBiasPower1Switch.Voltage() >= SP_MIN_ACVOLTAGE && 
			sat->SIGCondDriverBiasPower2Switch.Voltage() >= SP_MIN_ACVOLTAGE) {

			sat->SetAttitudeRotLevel(level);
			// Disable RJ/EC for the AGC++ control loop
			sat->rjec.SetAGCActiveTimer(2.0); //ApolloGuidance DELTAT
		}
	}
}

void CSMcomputer::LVGuidanceSwitchToggled(PanelSwitchItem *s) {
	ChannelValue30 val30;

	val30.Value = GetInputChannel(030); // Get current data

	if (s->GetState() == TOGGLESWITCH_UP) {
		val30.Bits.SCControlOfSaturn = 0;
	} else {
		val30.Bits.SCControlOfSaturn = 1;
	}
	SetInputChannel(030, val30.Value);
}



//
// CM Optics class code
//

CMOptics::CMOptics() {

	sat = NULL;
	OpticsShaft = 0.0;
	SextTrunion = 0.0;
	TeleTrunion = 0.0;
	ShaftMoved = 0.0;
	TrunionMoved = 0.0;
	OpticsManualMovement = 0;
	Powered = 0;
	SextDualView = false;
	SextDVLOSTog = false;
	SextDVTimer = 0.0;
	OpticsCovered = true;

	TargetShaft = 0;
	TargetTrunion = 0;
}

void CMOptics::Init(Saturn *vessel) {

	sat = vessel;
}

void CMOptics::SystemTimestep(double simdt) {

	// Optics system apparently uses 124.4 watts of power to operate.
	// This should probably vary up and down when the motors run, but I couldn't find data for it.
	Powered = 0; // Reset
	if (sat->GNOpticsMnACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE){
		Powered |= 1;
	}
	if (sat->GNOpticsMnBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE){
		Powered |= 2;
	}
	if (sat->GNPowerOpticsSwitch.IsDown()) {
		Powered = 0;
	}
	switch(Powered){
		case 0: // OFF
			break;
		case 1: // MNA
			sat->GNOpticsMnACircuitBraker.DrawPower(124.4);
			break;
		case 2: // MNB
			sat->GNOpticsMnBCircuitBraker.DrawPower(124.4);
			break;
		case 3: // BOTH
			sat->GNOpticsMnACircuitBraker.DrawPower(62.2);
			sat->GNOpticsMnBCircuitBraker.DrawPower(62.2);
			break;
	}

}

void CMOptics::CMCTrunionDrive(int val,int ch12) {

	int pulses;
	ChannelValue12 val12;
	val12.Value = ch12;

	if (Powered == 0) { return; }

	if (val&040000){ // Negative
		pulses = -((~val)&07777); 
	} else {
		pulses = val&07777; 
	}
	if (val12.Bits.EnableOpticsCDUErrorCounters){
		sat->agc.vagc.Erasable[0][RegOPTY] += pulses;
		sat->agc.vagc.Erasable[0][RegOPTY] &= 077777;
	}
	SextTrunion += (OCDU_TRUNNION_STEP*pulses); 
	TrunionMoved = SextTrunion;
	// sprintf(oapiDebugString(),"TRUNNION: %o PULSES, POS %o", pulses&077777 ,sat->agc.vagc.Erasable[0][035]);		
}

void CMOptics::CMCShaftDrive(int val,int ch12) {

	int pulses;
	ChannelValue12 val12;
	val12.Value = ch12;

	if (Powered == 0) { return; }

	if (val&040000){ // Negative
		pulses = -((~val)&07777); 
	} else {
		pulses = val&07777; 
	}
	OpticsShaft += (OCDU_SHAFT_STEP*pulses);
	ShaftMoved = OpticsShaft;
	if (val12.Bits.EnableOpticsCDUErrorCounters){
		sat->agc.vagc.Erasable[0][RegOPTX] += pulses;
		sat->agc.vagc.Erasable[0][RegOPTX] &= 077777;
	}
	// sprintf(oapiDebugString(),"SHAFT: %o PULSES, POS %o", pulses&077777, sat->agc.vagc.Erasable[0][036]);
}

void CMOptics::TimeStep(double simdt) {

	double ShaftRate = 0;
	double TrunRate = 0;

	SextDVTimer = SextDVTimer+simdt;
	if (SextDVTimer >= 0.06666){
		SextDVTimer = 0.0;
		SextDVLOSTog=!SextDVLOSTog;
	}

	// Optics cover handling
	if (OpticsCovered && sat->GetStage() >= STAGE_ORBIT_SIVB) {		
		if (OpticsShaft > 150. * RAD) {
			OpticsCovered = false;			
			sat->SetOpticsCoverMesh();
			sat->JettisonOpticsCover();
		}
	}

	if (Powered == 0) { return; }

	// Generate rates for telescope and manual mode
	switch(sat->ControllerSpeedSwitch.GetState()) {
		case THREEPOSSWITCH_UP:       // HI
			ShaftRate = 1775. * simdt;
			TrunRate  = 3640. * simdt;
			break;
		case THREEPOSSWITCH_CENTER:   // MED
			ShaftRate = 182. * simdt;
			TrunRate  = 364. * simdt;
			break;
		case THREEPOSSWITCH_DOWN:     // LOW
			ShaftRate = 18. * simdt;
			TrunRate  = 36. * simdt;
			break;
	}

	switch(sat->ModeSwitch.GetState()) {
		case THREEPOSSWITCH_DOWN: // ZERO OPTICS
			// Force MANUAL HI rate for zero optics mode.
			ShaftRate = 1775. * simdt;
			TrunRate = 3640. * simdt;

			if(OpticsShaft > 0){
				if(OpticsShaft > OCDU_SHAFT_STEP*ShaftRate){
					OpticsShaft -= OCDU_SHAFT_STEP*ShaftRate;
					ShaftMoved = OpticsShaft;
				}else{
					OpticsShaft = 0;
					ShaftMoved = 0;
				}
			}
			if(OpticsShaft < 0){
				if(OpticsShaft < (-OCDU_SHAFT_STEP*ShaftRate)){
					OpticsShaft += OCDU_SHAFT_STEP*ShaftRate;
					ShaftMoved = OpticsShaft;
				}else{
					OpticsShaft = 0;
					ShaftMoved = 0;
				}
			}
			if(SextTrunion > 0){
				if(SextTrunion > OCDU_TRUNNION_STEP*TrunRate){
					SextTrunion -= OCDU_TRUNNION_STEP*TrunRate;
					TrunionMoved = SextTrunion;
				}else{
					SextTrunion = 0;
					TrunionMoved = 0;
				}				
			}
			if(SextTrunion < 0){
				if(SextTrunion < (-OCDU_TRUNNION_STEP*TrunRate)){
					SextTrunion += OCDU_TRUNNION_STEP*TrunRate;
					TrunionMoved = SextTrunion;
				}else{
					SextTrunion = 0;
					TrunionMoved = 0;
				}				
			}
			break;
		case THREEPOSSWITCH_CENTER: // MANUAL

			/* About "SextTrunion < (RAD*59.0)":

			# Page 711
			# PROGRAM NAME -- PICAPAR   DATE: DEC 20 66
			# MOD 1            LOG SECTION: P51-P53
			#            ASSEMBLY:  SUNDISK REV40
			# BY KEN VINCENT
			#
			# FUNCTION
			#   THIS PROGRAM READS THE IMU-CDUS AND COMPUTES THE VEHICLE ORIENTATION
			#   WITH RESPECT TO INERTIAL SPACE.  IT THEN COMPUTES THE SHAFT AXIS (SAX)
			#   WITH RESPECT TO REFERENCE INTERTIAL.  EACH STAR IN THE CATALOG IS TESTED
			#    TO DETERMINE IF IT IS OCCULTED BY EITHER EARTH, SUN OR MOON.  IF A
			#    STAR IS NOT OCCULTED THEN IT IS PARIED WITH ALL STARS OF LOWER INDEX.
			#    THE PAIRED STAR IS TESTED FOR OCCULTATION.  PAIRS OF STARS THAT PASS
			#   THE OCCULTATION TESTS ARE TESTED FOR GOOD SEPARATION.  A PAIR OF STARS
			#   HAVE GOOD SEPARATION IF THE ANGLE BETWEEN THEM IS LESS THAN 66 DEGREES
			#   AND MORE THAN 40 DEGREES.  THOSE PAIRS WITH GOOD SEPARATION
			#   ARE THEN TESTED TO SEE IF THEY LIE IN CURRENT FIELD OF VIEW.  (WITHIN
			#   33 DEGREES OF SAX).  THE PAIR WITH MAX SEPARATION IS CHOSEN FROM
			#   THOSE WITH GOOD SEPARATION, AND IN FIELD OF VIEW.

			As stated above, the angular difference between the 2 stars should be 40°-66°, a rather tight margin. 
			Greater than 66° isn't working at all, but smaller than 40° is at least possible (i.e. no errors) however less precise.

			But the stated max. field of view = max. trunnion angle of 33° seems to be wrong. 
			With trunnion angles greater than 60°, P51 isn't working anymore (as I figured out by testing). 
			But PICAPAR does choose stars with a trunnion angle greater than 33° (current "record" 45°), so restricting the max. trunnion
			angle to 33° is NO option. AOH 2.2.3.3.1 seems to state a max. trunnion angle of 50°.

			http://www.ibiblio.org/mscorbit/mscforum/index.php?topic=2514.msg20287#msg20287
			*/

			if ((OpticsManualMovement&0x01) != 0 && SextTrunion < (RAD*59.0)) {
				SextTrunion += OCDU_TRUNNION_STEP * TrunRate;				
				while (fabs(fabs(SextTrunion)-fabs(TrunionMoved)) >= OCDU_TRUNNION_STEP) {					
					sat->agc.vagc.Erasable[0][RegOPTY]++;
					sat->agc.vagc.Erasable[0][RegOPTY] &= 077777;
					TrunionMoved += OCDU_TRUNNION_STEP;
				}
			}
			if ((OpticsManualMovement&0x02) != 0 && SextTrunion > 0) {
				SextTrunion -= OCDU_TRUNNION_STEP * TrunRate;				
				while(fabs(fabs(SextTrunion)-fabs(TrunionMoved)) >= OCDU_TRUNNION_STEP) {					
					sat->agc.vagc.Erasable[0][RegOPTY]--;
					sat->agc.vagc.Erasable[0][RegOPTY] &= 077777;
					TrunionMoved -= OCDU_TRUNNION_STEP;
				}
			}
			if ((OpticsManualMovement&0x04) != 0 && OpticsShaft > -(RAD*270.0)) {
				OpticsShaft -= OCDU_SHAFT_STEP * ShaftRate;					
				while(fabs(fabs(OpticsShaft)-fabs(ShaftMoved)) >= OCDU_SHAFT_STEP) {
					sat->agc.vagc.Erasable[0][RegOPTX]--;
					sat->agc.vagc.Erasable[0][RegOPTX] &= 077777;
					ShaftMoved -= OCDU_SHAFT_STEP;
				}
			}
			if ((OpticsManualMovement&0x08) != 0 && OpticsShaft < (RAD*270.0)) {
				OpticsShaft += OCDU_SHAFT_STEP * ShaftRate;					
				while(fabs(fabs(OpticsShaft)-fabs(ShaftMoved)) >= OCDU_SHAFT_STEP) {
					sat->agc.vagc.Erasable[0][RegOPTX]++;
					sat->agc.vagc.Erasable[0][RegOPTX] &= 077777;
					ShaftMoved += OCDU_SHAFT_STEP;
				}
			}
			break;
	}

	// TELESCOPE TRUNNION MAINTENANCE (happens in all modes)
	// If the CMC issued pulses, they will have happened before we got here, so the sextant angle will be right.
	// If the order of timestep() calls is changed, this will "lag".

	double TeleTrunionTarget = 0;
	switch(sat->ControllerTelescopeTrunnionSwitch.GetState()){
		case THREEPOSSWITCH_UP:			// SLAVE TO SEXTANT			
			TeleTrunionTarget = SextTrunion;
			break;
		case THREEPOSSWITCH_CENTER:		// 0 DEG
			TeleTrunionTarget = 0;
			break;
		case THREEPOSSWITCH_DOWN:		// OFFSET 25 DEG
			TeleTrunionTarget = SextTrunion + 0.218166156; // Add 12.5 degrees to sextant angle
			break;
	}
	if(TeleTrunion > TeleTrunionTarget){
		if(TeleTrunion > TeleTrunionTarget-(OCDU_TRUNNION_STEP*TrunRate)){
			TeleTrunion -= OCDU_TRUNNION_STEP*TrunRate;				
		}else{
			TeleTrunion = TeleTrunionTarget;
		}				
	}
	if(TeleTrunion < TeleTrunionTarget){
		if(TeleTrunion < TeleTrunionTarget+(-OCDU_TRUNNION_STEP*TrunRate)){
			TeleTrunion += OCDU_TRUNNION_STEP*TrunRate;
		}else{
			TeleTrunion = TeleTrunionTarget;
		}				
	}

	//sprintf(oapiDebugString(), "Optics Shaft %.2f, Sext Trunion %.2f, Tele Trunion %.2f", OpticsShaft/RAD, SextTrunion/RAD, TeleTrunion/RAD);
	//sprintf(oapiDebugString(), "Sext Trunion EMEM %o", sat->agc.vagc.Erasable[0][RegOPTY]);
}

void CMOptics::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, CMOPTICS_START_STRING);
	oapiWriteScenario_int(scn, "POWERED", Powered);
	oapiWriteScenario_int(scn, "OPTICSMANUALMOVEMENT", OpticsManualMovement);
	papiWriteScenario_double(scn, "OPTICSSHAFT", OpticsShaft);
	papiWriteScenario_double(scn, "SEXTTRUNION", SextTrunion);
	papiWriteScenario_double(scn, "TELETRUNION", TeleTrunion);
	papiWriteScenario_double(scn, "TARGETSHAFT", TargetShaft);
	papiWriteScenario_double(scn, "TARGETTRUNION", TargetTrunion);
	papiWriteScenario_double(scn, "SHAFTMOVED", ShaftMoved);
	papiWriteScenario_double(scn, "TRUNIONMOVED", TrunionMoved);
	papiWriteScenario_bool(scn, "OPTICSCOVERED", OpticsCovered); 
	oapiWriteLine(scn, CMOPTICS_END_STRING);
}

void CMOptics::LoadState(FILEHANDLE scn) {

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, CMOPTICS_END_STRING, sizeof(CMOPTICS_END_STRING)))
			return;
		else if (!strnicmp (line, "POWERED", 7)) {
			sscanf (line+7, "%d", &Powered);
		}
		else if (!strnicmp (line, "OPTICSMANUALMOVEMENT", 20)) {
			sscanf (line+20, "%d", &OpticsManualMovement);
		}
		else if (!strnicmp (line, "OPTICSSHAFT", 11)) {
			sscanf (line+11, "%lf", &OpticsShaft);
		}
		else if (!strnicmp (line, "SEXTTRUNION", 11)) {
			sscanf (line+11, "%lf", &SextTrunion);
		}
		else if (!strnicmp (line, "TELETRUNION", 11)) {
			sscanf (line+11, "%lf", &TeleTrunion);
		}
		else if (!strnicmp (line, "TARGETSHAFT", 11)) {
			sscanf (line+11, "%lf", &TargetShaft);
		}
		else if (!strnicmp (line, "TARGETTRUNION", 13)) {
			sscanf (line+13, "%lf", &TargetTrunion);
		}
		else if (!strnicmp (line, "SHAFTMOVED", 10)) {
			sscanf (line+10, "%lf", &ShaftMoved);
		}
		else if (!strnicmp (line, "TRUNIONMOVED", 12)) {
			sscanf (line+12, "%lf", &TrunionMoved);
		} 
		papiReadScenario_bool(line, "OPTICSCOVERED", OpticsCovered); 
	}
}
