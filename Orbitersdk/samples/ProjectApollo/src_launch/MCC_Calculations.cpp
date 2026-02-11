/***************************************************************************
  This file is part of Project Apollo - NASSP

  MCC Calculations

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

#include "MCC_Calculations.h"
#include "nassputils.h"
#include "soundlib.h"
#include "apolloguidance.h"
#include "saturn.h"
#include "iu.h"
#include "sivb.h"
#include "LVDC.h"
#include "rtcc.h"

using namespace nassp;

MCC_Calculations::MCC_Calculations(RTCC *r) : RTCCModule(r)
{

}

bool MCC_Calculations::CreateEphemeris(EphemerisData sv, double EphemerisLeftLimitGMT, double EphemerisRightLimitGMT, EphemerisDataTable2 &ephem)
{
	//INPUTS:
	//sv: State vector. Reference body indicator used to either generate Earth or Moon centered inertial (ECI/MCI) ephemeris.
	//EphemerisLeftLimitGMT: Lower GMT limit of desired ephemeris
	//EphemerisRightLimitGMT: Upper GMT limit of desired ephemeris
	//OUTPUTS:
	//ephem: Ephemeris table
	//return value: true = error return 

	PLAWDTOutput weights;

	weights.KFactor = 0.0;
	weights.CC.set(0);
	weights.CSMWeight = 1.0;
	weights.CSMArea = 0.0;

	return CreateEphemeris(sv, weights, EphemerisLeftLimitGMT, EphemerisRightLimitGMT, ephem);
}

bool MCC_Calculations::CreateEphemeris(VehicleDataBlock sv, double EphemerisLeftLimitGMT, double EphemerisRightLimitGMT, EphemerisDataTable2& ephem)
{
	//INPUTS:
	//sv: State vector and weights. Reference body indicator used to either generate Earth or Moon centered inertial (ECI/MCI) ephemeris.
	//EphemerisLeftLimitGMT: Lower GMT limit of desired ephemeris
	//EphemerisRightLimitGMT: Upper GMT limit of desired ephemeris
	//OUTPUTS:
	//ephem: Ephemeris table
	//return value: true = error return 

	PLAWDTOutput weights;

	weights.KFactor = sv.KFactor;
	weights.CC.set(0);
	weights.CSMWeight = sv.Weight;
	weights.CSMArea = sv.Area;

	return CreateEphemeris(sv.sv, weights, EphemerisLeftLimitGMT, EphemerisRightLimitGMT, ephem);
}

bool MCC_Calculations::CreateEphemeris(EphemerisData sv, PLAWDTOutput weights, double EphemerisLeftLimitGMT, double EphemerisRightLimitGMT, EphemerisDataTable2& ephem)
{
	//INPUTS:
	//sv: State vector. Reference body indicator used to either generate Earth or Moon centered inertial (ECI/MCI) ephemeris.
	//weights = Weight table
	//EphemerisLeftLimitGMT: Lower GMT limit of desired ephemeris
	//EphemerisRightLimitGMT: Upper GMT limit of desired ephemeris
	//OUTPUTS:
	//ephem: Ephemeris table
	//return value: true = error return 

	EMSMISSInputTable in;

	in.AnchorVector = sv;
	in.EphemerisLeftLimitGMT = EphemerisLeftLimitGMT;
	in.EphemerisRightLimitGMT = EphemerisRightLimitGMT;

	in.EphemerisBuildIndicator = true;
	if (sv.RBI == BODY_EARTH)
	{
		in.ECIEphemerisIndicator = true;
		in.ECIEphemTableIndicator = &ephem;
	}
	else
	{
		in.MCIEphemerisIndicator = true;
		in.MCIEphemTableIndicator = &ephem;
	}
	in.useInputWeights = true;
	in.WeightsTable = &weights;
	in.VehicleCode = RTCC_MPT_CSM; //Not used

	pRTCC->EMSMISS(&in);

	ephem.Header.TUP = 1; //Only has to be non-zero

	return (in.NIAuxOutputTable.ErrorCode != 0);
}

double MCC_Calculations::EnvironmentChange(EphemerisDataTable2 &ephem, double gmt_estimate, int option, bool present, bool terminator)
{
	//INPUTS:
	//ephem: Ephemeris table
	//gmt_estimate: Estimated GMT for environment change, used as threshold
	//option: 0 = Sun, 1 = Moon
	//present: Search for condition being present (e.g. sunshine for sunrise)
	//terminator: Search for terminator of above condition
	//OUTPUTS:
	//return value: GMT of environment change

	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNRSTAY;
	RTCC::EMMENVInputTable in;
	RTCC::EMMENVOutputTable out;

	in.GMT = gmt_estimate;
	in.option = option;
	in.present = present;
	in.terminator = terminator;

	pRTCC->EMMENV(ephem, MANTIMES, &LUNRSTAY, in, out);

	return out.T_Change;
}

double MCC_Calculations::Sunrise(EphemerisDataTable2 &ephem, double gmt_estimate)
{
	return EnvironmentChange(ephem, gmt_estimate, 0, true, false);
}

double MCC_Calculations::TerminatorRise(EphemerisDataTable2 &ephem, double gmt_estimate)
{
	return EnvironmentChange(ephem, gmt_estimate, 0, true, true);
}

bool MCC_Calculations::LongitudeCrossing(EphemerisDataTable2 &ephem, double lng, double gmt_estimate, double &gmt_cross)
{
	//INPUTS:
	//ephem: Ephemeris table (ECI or MCI coordinates)
	//lng: Desired longitude
	//gmt_estimate: Estimated GMT for environment change, used as threshold
	//OUTPUTS:
	//gmt_cross: Calculated GMT of longitude crossing
	//return value: true = error return

	ManeuverTimesTable MANTIMES;
	EphemerisDataTable2 ephem_true;
	EphemerisData2 sv;
	double dErr;
	int in, out, iErr;

	gmt_cross = 0.0;

	//Convert to ECT or MCT table
	ephem_true = ephem;

	in = ephem.Header.CSI;
	out = in + 1;

	iErr = pRTCC->ELVCNV(ephem.table, in, out, ephem_true.table);
	if (iErr) return true;

	ephem_true.Header.CSI = out;

	//Longitude crossing
	dErr = pRTCC->RLMTLC(ephem_true, MANTIMES, lng, gmt_estimate, gmt_cross, sv);

	return (dErr < 0.0);
}

bool MCC_Calculations::GETEval(double get)
{
	if (OrbMech::GETfromMJD(oapiGetSimMJD(), pRTCC->CalcGETBase()) > get)
	{
		return true;
	}

	return false;
}

double MCC_Calculations::ComputeDVTO(double mass) //mass in kg
{
	double DVTO;
	DVTO = (pRTCC->SystemParameters.MCTST5 * pRTCC->SystemParameters.MCTSD5) / mass;
	return DVTO*3.28084; //Convert to ft/s
}

double MCC_Calculations::FindOrbitalSunrise(VehicleDataBlock sv, double t_sunrise_guess)
{
	//Temporary conversion function
	SV sv2;
	sv2 = pRTCC->ConvertEphemDatatoSV(sv.sv, sv.Weight);
	return FindOrbitalSunrise(sv2, t_sunrise_guess);
}

double MCC_Calculations::FindOrbitalSunrise(SV sv, double t_sunrise_guess)
{
	SV sv1;
	double GET_SV, dt, ttoSunrise;

	OBJHANDLE hSun = oapiGetObjectByName("Sun");

	GET_SV = OrbMech::GETfromMJD(sv.MJD, pRTCC->CalcGETBase());
	dt = t_sunrise_guess - GET_SV;

	sv1 = pRTCC->coast(sv, dt);

	ttoSunrise = OrbMech::sunrise(pRTCC->SystemParameters.MAT_J2000_BRCS, sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, true, false, false);
	return t_sunrise_guess + ttoSunrise;
}

double MCC_Calculations::FindOrbitalSunset(VehicleDataBlock sv, double t_sunset_guess)
{
	//Temporary conversion function
	SV sv2;
	sv2 = pRTCC->ConvertEphemDatatoSV(sv.sv, sv.Weight);
	return FindOrbitalSunset(sv2, t_sunset_guess);
}

double MCC_Calculations::FindOrbitalSunset(SV sv, double t_sunset_guess)
{
	SV sv1;
	double GET_SV, dt, ttoSunset;

	OBJHANDLE hSun = oapiGetObjectByName("Sun");

	GET_SV = OrbMech::GETfromMJD(sv.MJD, pRTCC->CalcGETBase());
	dt = t_sunset_guess - GET_SV;

	sv1 = pRTCC->coast(sv, dt);

	ttoSunset = OrbMech::sunrise(pRTCC->SystemParameters.MAT_J2000_BRCS, sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, false, false, false);
	return t_sunset_guess + ttoSunset;
}

double MCC_Calculations::FindOrbitalMidnight(SV sv, double t_TPI_guess)
{
	SV sv1;
	double GET_SV, dt, ttoMidnight;

	OBJHANDLE hSun = oapiGetObjectByName("Sun");

	GET_SV = OrbMech::GETfromMJD(sv.MJD, pRTCC->CalcGETBase());
	dt = t_TPI_guess - GET_SV;

	sv1 = pRTCC->coast(sv, dt);

	ttoMidnight = OrbMech::sunrise(pRTCC->SystemParameters.MAT_J2000_BRCS, sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, 1, 1, false);
	return t_TPI_guess + ttoMidnight;
}

int MCC_Calculations::StationContactsGenerator(EphemerisDataTable2& ephem, double lat, double lng, double alt, int RBI, StationContact& contact)
{
	//INPUTS:
	//ephem: Ephemeris data table generated with CreateEphemeris (ECI or MCI coordinates). Does not have to agree with RBI
	//lat: Latitude of the ground station, radians
	//lng: Longitude of the ground station, radians
	//alt: Elevation of the ground station, radians
	//RBI: Reference body indicator of the ground station (0 = Earth, 1 = Moon)
	//OUTPUTS:
	//contact: Station contact data table
	//return value: Error if non-zero

	EphemerisDataTable2 ephem_true;
	std::vector<StationContact> acquisitions;
	ManeuverTimesTable mantimes;
	StationData station;
	int csi, error;

	pRTCC->EMGGPCHR(lat, lng, alt, RBI, 0.0, &station);

	//Convert ephemeris to ECT or MCT
	if (RBI == BODY_EARTH)
	{
		csi = RTCC_COORDINATES_ECT;
	}
	else
	{
		csi = RTCC_COORDINATES_MCT;
	}
	error = pRTCC->ELVCNV(ephem.table, ephem.Header.CSI, csi, ephem_true.table);
	if (error) return 1;
	ephem_true.Header = ephem.Header;
	ephem_true.Header.CSI = csi;

	//Generate station contact
	error = pRTCC->EMXING(ephem_true, mantimes, station, RBI, acquisitions, NULL, 1);
	if (error || (acquisitions.size() == 0U)) return 2;

	contact = acquisitions[0];
	return 0;
}

int MCC_Calculations::GroundTargetPointing(EphemerisDataTable2& ephem, MATRIX3 REFSMMAT, double gmt, double lat, double lng, double alt, int RBI, double yaw, double pitch, double omicron, VECTOR3& Att)
{
	//INPUTS:
	//ephem: Ephemeris data table generated with CreateEphemeris (ECI or MCI coordinates). Does not have to agree with RBI.
	//REFSMMAT: REFSMMAT of vehicle for which the attitude is to be calculated
	//gmt: Time of sighting
	//lat: Latitude of ground target, radians
	//lng: Longitude of ground target, radians
	//alt: Elevation of ground target, meters
	//RBI: Reference body indicator for ground target (0 = Earth, 1 = Moon)
	//yaw: Yaw angle, radians. Same definition as in P20 universal tracking
	//pitch: Pitch angle, radians. Same definition as in P20 universal tracking
	//omicron: Azimuth angle, radians. Same definition as in P20 universal tracking
	//OUTPUTS:
	//Att: Attitude for sighting
	//return value: error if non-zero

	ELVCTRInputTable elin;
	ELVCTROutputTable2 elout;
	ManeuverTimesTable mantimes;
	VECTOR3 R_iner, u_LOS;
	double Elev;
	int err, csi;
	bool HasLOS;

	//Interpolate for vector
	elin.GMT = gmt;
	pRTCC->ELVCTR(elin, elout, ephem, mantimes);
	if (elout.ErrorCode > 2) return 1;

	//Convert to RBI, if required
	if (RBI == BODY_EARTH)
	{
		csi = RTCC_COORDINATES_ECI;
	}
	else
	{
		csi = RTCC_COORDINATES_MCI;
	}
	err = pRTCC->ELVCNV(elout.SV, ephem.Header.CSI, csi, elout.SV);
	if (err) return 1;

	//Find direction
	err = pRTCC->EMGSDEMT(elout.SV, RBI, lat, lng, alt, R_iner, u_LOS, HasLOS, Elev);
	if (err) return 1;

	//Calculate attitude
	Att = AttitudeFromPointingDirection(elout.SV.R, elout.SV.V, REFSMMAT, u_LOS, yaw, pitch, omicron);

	return 0;
}

int MCC_Calculations::CelestialTargetPointing(EphemerisDataTable2& ephem, MATRIX3 REFSMMAT, double gmt, int star, double yaw, double pitch, double omicron, VECTOR3& Att)
{
	//INPUTS:
	//ephem: Ephemeris data table generated with CreateEphemeris (ECI or MCI coordinates)
	//REFSMMAT: REFSMMAT of vehicle for which the attitude is to be calculated
	//gmt: Time of sighting
	//star: Star from star table (1-400)
	//yaw: Yaw angle, radians. Same definition as in P20 universal tracking
	//pitch: Pitch angle, radians. Same definition as in P20 universal tracking
	//omicron: Azimuth angle, radians. Same definition as in P20 universal tracking
	//OUTPUTS:
	//Att: Attitude for sighting
	//return value: error if non-zero

	ELVCTRInputTable elin;
	ELVCTROutputTable2 elout;
	ManeuverTimesTable mantimes;
	VECTOR3 u_LOS;

	//Interpolate for vector
	elin.GMT = gmt;
	pRTCC->ELVCTR(elin, elout, ephem, mantimes);
	if (elout.ErrorCode > 2) return 1;

	//Get line-of-sight vector
	if (star < 0 || star > 400) return 1;
	u_LOS = pRTCC->EZJGSTAR[star - 1];

	//Calculate attitude
	Att = AttitudeFromPointingDirection(elout.SV.R, elout.SV.V, REFSMMAT, u_LOS, yaw, pitch, omicron);

	return 0;
}

int MCC_Calculations::CelestialBodyPointing(EphemerisDataTable2& ephem, MATRIX3 REFSMMAT, double gmt, int option, double yaw, double pitch, double omicron, VECTOR3& Att)
{
	//INPUTS:
	//ephem: Ephemeris data table generated with CreateEphemeris (ECI or MCI coordinates)
	//REFSMMAT: REFSMMAT of vehicle for which the attitude is to be calculated
	//gmt: Time of sighting
	//option: 0 = Center of Earth, 1 = center of Moon, 2 = center of Sun
	//yaw: Yaw angle, radians. Same definition as in P20 universal tracking
	//pitch: Pitch angle, radians. Same definition as in P20 universal tracking
	//omicron: Azimuth angle, radians. Same definition as in P20 universal tracking
	//OUTPUTS:
	//Att: Attitude for sighting
	//return value: error if non-zero

	ELVCTRInputTable elin;
	ELVCTROutputTable2 elout;
	ManeuverTimesTable mantimes;
	VECTOR3 u_LOS;
	int err;

	//Interpolate for vector
	elin.GMT = gmt;
	pRTCC->ELVCTR(elin, elout, ephem, mantimes);
	if (elout.ErrorCode > 2) return 1;

	//Get line-of-sight vector
	err = CelestialBodyPointingDirection(elout.SV.R, elout.SV.GMT, ephem.Header.CSI, option, u_LOS);
	if (err) return 1;

	//Calculate attitude
	Att = AttitudeFromPointingDirection(elout.SV.R, elout.SV.V, REFSMMAT, u_LOS, yaw, pitch, omicron);

	return 0;
}

int MCC_Calculations::CelestialBodyPointingDirection(VECTOR3 R, double GMT, int CSI, int option, VECTOR3& u_LOS)
{
	//INPUTS:
	//R = Position vector
	//GMT: Time of sighting
	//CSI: Coordinate system indicator for position vector
	//option: 0 = Center of Earth, 1 = center of Moon, 2 = center of Sun
	//OUTPUTS:
	//u_LOS: Inertial line-of-sight unit vector from vehicle to body
	//return value: error if non-zero

	VECTOR3 R_EM, V_EM, R_ES, R_VB;
	int err;

	//Get ephemerides
	err = pRTCC->PLEFEM(1, GMT / 3600.0, 0, &R_EM, &V_EM, &R_ES, NULL);
	if (err) return err;

	if (CSI == RTCC_COORDINATES_ECI)
	{
		if (option == 0)
		{
			R_VB = -R;
		}
		else if (option == 1)
		{
			R_VB = R_EM - R;
		}
		else
		{
			R_VB = R_ES - R;
		}
	}
	else
	{
		if (option == 0)
		{
			R_VB = -R_EM - R;
		}
		else if (option == 1)
		{
			R_VB = -R;
		}
		else
		{
			R_VB = -R_EM + R_ES - R;
		}
	}
	u_LOS = unit(R_VB);
	return 0;
}

VECTOR3 MCC_Calculations::AttitudeFromPointingDirection(VECTOR3 R, VECTOR3 V, MATRIX3 REFSMMAT, VECTOR3 u_LOS, double yaw, double pitch, double omicron)
{
	MATRIX3 RFNB;
	VECTOR3 SCAXIS;

	//Build body pointing vector
	SCAXIS = _V(cos(yaw) * cos(pitch), sin(yaw) * cos(pitch), -sin(pitch));
	//Calculate reference to navigation base matrix
	RFNB = OrbMech::THREEAXISPOINTING(R, V, SCAXIS, u_LOS, omicron);
	//Calculate attitude
	return OrbMech::CALCGAR(REFSMMAT, RFNB);
}

void MCC_Calculations::FindRadarAOSLOS(SV sv, double lat, double lng, double &GET_AOS, double &GET_LOS)
{
	VECTOR3 R_P;
	double LmkRange, dt1, dt2;

	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)))*oapiGetSize(sv.gravref);

	dt1 = OrbMech::findelev_gs(pRTCC->SystemParameters.AGCEpoch, pRTCC->SystemParameters.MAT_J2000_BRCS, sv.R, sv.V, R_P, sv.MJD, 175.0*RAD, sv.gravref, LmkRange);
	dt2 = OrbMech::findelev_gs(pRTCC->SystemParameters.AGCEpoch, pRTCC->SystemParameters.MAT_J2000_BRCS, sv.R, sv.V, R_P, sv.MJD, 5.0*RAD, sv.gravref, LmkRange);

	GET_AOS = OrbMech::GETfromMJD(sv.MJD, pRTCC->CalcGETBase()) + dt1;
	GET_LOS = OrbMech::GETfromMJD(sv.MJD, pRTCC->CalcGETBase()) + dt2;
}

int MCC_Calculations::SPSRCSDecision(double a, VECTOR3 dV_LVLH)
{
	double t;

	t = length(dV_LVLH) / a;

	if (t > 0.5)
	{
		return RTCC_ENGINETYPE_CSMSPS;
	}
	else
	{
		return RTCC_ENGINETYPE_CSMRCSPLUS4;
	}
}

bool MCC_Calculations::REFSMMATDecision(VECTOR3 Att)
{
	if (cos(Att.z) > 0.5) //Yaw between 300° and 60°
	{
		return true;
	}

	return false;
}

void MCC_Calculations::BackupGDCAlignment(VehicleDataBlock sv, double GET, MATRIX3 REFSMMAT, int PrefGDCStars, VECTOR3 &GDCangles, char *SetStars)
{
	//INPUTS:
	//sv: State vector
	//GET: Time of alignment
	//REFSMMAT: REFSMMAT to be aligned to
	//PrefGDCStars: Preferred star set for the GDC backup alignment. 0 = Deneb, Vega, 1 = Navi, Polaris, 2 = Acrux, Atria, 3 = Sirius, Rigel
	//OUTPUTS:
	//GDCangles: Backup GDC alignment angles in degrees (all zeros if no unocculted star set was found)
	//SetStars: String with the calculated star set (or N/A if no set was found)

	VehicleDataBlock sv_sxt;
	double GMT, R_E;
	int GDCset;

	//Calculate GMT from input GET
	GMT = pRTCC->GMTfromGET(GET);
	//Propagate state vector to alignment time
	sv_sxt = pRTCC->coast(sv, GMT - sv.sv.GMT);

	//Get body radius
	if (sv_sxt.sv.RBI == BODY_EARTH)
	{
		R_E = OrbMech::R_Earth;
	}
	else
	{
		R_E = OrbMech::R_Moon;
	}
	//Calculate backup GDC angles
	GDCangles = OrbMech::backupgdcalignment(pRTCC->EZJGSTAR, REFSMMAT, sv_sxt.sv.R, R_E, PrefGDCStars, GDCset);
	//Write output string
	if (length(GDCangles) == 0.0)
	{
		sprintf(SetStars, "N/A");
	}
	else
	{
		//Convert to full IMU angles in degrees
		GDCangles = _V(OrbMech::imulimit(GDCangles.x*DEG), OrbMech::imulimit(GDCangles.y*DEG), OrbMech::imulimit(GDCangles.z*DEG));
		if (GDCset == 0)
		{
			sprintf(SetStars, "Deneb, Vega");
		}
		else if (GDCset == 1)
		{
			sprintf(SetStars, "Navi, Polaris");
		}
		else if (GDCset == 2)
		{
			sprintf(SetStars, "Acrux, Atria");
		}
		else
		{
			sprintf(SetStars, "Sirius, Rigel");
		}
	}
}

void MCC_Calculations::PrelaunchMissionInitialization()
{
	//Assumes mission file has been loaded. Also GZGENCSN.MonthofLiftoff, GZGENCSN.DayofLiftoff, GZGENCSN.Year in the scenario.

	char Buff[128];

	//P80 MED: mission initialization
	sprintf_s(Buff, "P80,1,CSM,%d,%d,%d;", pRTCC->GZGENCSN.MonthofLiftoff, pRTCC->GZGENCSN.DayofLiftoff, pRTCC->GZGENCSN.Year);
	pRTCC->GMGMED(Buff);
}

double MCC_Calculations::GetLVDCOrbitalInsertionTime(VESSEL *v)
{
	if (utils::IsVessel(v, utils::SaturnIB) || utils::IsVessel(v, utils::SaturnIB_SIVB))
	{
		//Saturn IB LVDC
		LVDC1B *lvdc;

		if (utils::IsVessel(v, utils::SaturnIB))
		{
			Saturn *sat = (Saturn*)v;
			lvdc = (LVDC1B*)sat->GetIU()->GetLVDC();
		}
		else
		{
			SIVB *sivb = (SIVB*)v;
			lvdc = (LVDC1B*)sivb->GetIU()->GetLVDC();
		}

		return (lvdc->T_CO - 17.0); //TBD: Time when TB4 was established would be better, but it is not being saved yet
	}
	else if (utils::IsVessel(v, utils::SaturnV) || utils::IsVessel(v, utils::SaturnV_SIVB))
	{
		//Saturn V LVDC
		LVDCSV *lvdc;

		if (utils::IsVessel(v, utils::SaturnV))
		{
			Saturn *sat = (Saturn*)v;
			lvdc = (LVDCSV*)sat->GetIU()->GetLVDC();
		}
		else
		{
			SIVB *sivb = (SIVB*)v;
			lvdc = (LVDCSV*)sivb->GetIU()->GetLVDC();
		}

		return (lvdc->TB5 - 17.0);
	}
	else return 0.0;
}

void MCC_Calculations::DMissionRendezvousPlan(SV sv_A0, double &t_TPI0)
{
	SV sv2;

	//Step 1: Find TPI0 time (25 minutes before sunrise)
	double TPI0_guess, TPI0_sunrise_guess, TPI0_sunrise, dt_sunrise;
	dt_sunrise = 25.0*60.0;
	TPI0_guess = OrbMech::HHMMSSToSS(95, 0, 0);
	TPI0_sunrise_guess = TPI0_guess + dt_sunrise;
	TPI0_sunrise = FindOrbitalSunrise(sv_A0, TPI0_sunrise_guess);
	t_TPI0 = TPI0_sunrise - dt_sunrise;

	//Step 2: Phasing is 70 minutes before TPI0
	pRTCC->calcParams.Phasing = t_TPI0 - 70.0*60.0;

	//Step 3: Insertion is 111:42 minutes after Phasing
	pRTCC->calcParams.Insertion = pRTCC->calcParams.Phasing + 111.0*60.0 + 42.0;

	//Step 4: CSI is two minutes (rounded) after 5° AOS of the TAN pass
	double CSI_guess, lat_TAN, lng_TAN, AOS_TAN, LOS_TAN;
	lat_TAN = groundstations[13][0];
	lng_TAN = groundstations[13][1];
	CSI_guess = pRTCC->calcParams.Insertion + 40.0*60.0;
	sv2 = pRTCC->coast(sv_A0, CSI_guess - OrbMech::GETfromMJD(sv_A0.MJD, pRTCC->CalcGETBase()));
	FindRadarAOSLOS(sv2, lat_TAN, lng_TAN, AOS_TAN, LOS_TAN);
	pRTCC->calcParams.CSI = (floor(AOS_TAN / 60.0) + 2.0)*60.0;

	//Step 5: CDH is placed 44.4 minutes after CSI
	pRTCC->calcParams.CDH = pRTCC->calcParams.CSI + 44.4*60.0;

	//Step 6: Find TPI0 time (25 minutes before sunrise)
	double TPI_guess, TPI_sunrise_guess, TPI_sunrise;
	TPI_guess = OrbMech::HHMMSSToSS(98, 0, 0);
	TPI_sunrise_guess = TPI_guess + dt_sunrise;
	TPI_sunrise = FindOrbitalSunrise(sv_A0, TPI_sunrise_guess);
	pRTCC->calcParams.TPI = TPI_sunrise - dt_sunrise;
}

void MCC_Calculations::FMissionRendezvousPlan(VESSEL *chaser, VESSEL *target, SV sv_A0, double t_TIG, double t_TPI, double &t_Ins, double &CSI)
{
	//Plan: Phasing (fixed TIG), Insertion, CSI 50 minutes after Insertion, CDH, TPI at orbital midnight (Apollo 10)

	TwoImpulseOpt lamopt, lamopt2;
	TwoImpulseResuls lamres;
	double GETbase, t_sv0, t_Phasing, t_Insertion, dt, t_CSI, ddt, T_P, dv_CSI, t_CDH, dt_TPI, t_TPI_apo;
	VECTOR3 dV_Phasing, dV_Insertion, R_P_CDH1, V_P_CDH1;
	SV sv_P0, sv_P_CSI, sv_Phasing, sv_Phasing_apo, sv_Insertion, sv_Insertion_apo, sv_CSI, sv_CSI_apo, sv_CDH, sv_CDH_apo, sv_P_CDH;

	//Constants
	const double dt2 = 50.0*60.0; //Insertion to CSI
	const double DH = 15.0*1852.0;

	GETbase = pRTCC->CalcGETBase();
	t_Phasing = t_TIG;
	dt = 7017.0; //Phasing to Insertion
	dv_CSI = 50.0*0.3048;
	ddt = 10.0;

	sv_P0 = pRTCC->StateVectorCalc(target);

	lamopt.mode = 5; //External request
	lamopt.DH = 60.0*1852.0 - 60000.0*0.3048; //Aiming for 60000 ft altitude
	lamopt.PhaseAngle = 15.509*RAD; //270 NM behind CSM
	lamopt.T1 = pRTCC->GMTfromGET(t_Phasing);
	lamopt.sv_T.sv = pRTCC->ConvertSVtoEphemData(sv_P0);

	lamopt2 = lamopt;
	lamopt2.DH = 14.7*1852.0; //14.7 NM
	lamopt2.PhaseAngle = 8.4436*RAD; //147 NM behind

	t_sv0 = OrbMech::GETfromMJD(sv_A0.MJD, GETbase);
	sv_Phasing = pRTCC->coast(sv_A0, t_Phasing - t_sv0);

	//Loop
	while (abs(ddt) > 1.0)
	{
		//Phasing Targeting
		t_Insertion = t_Phasing + dt;

		lamopt.T2 = pRTCC->GMTfromGET(t_Insertion);
		lamopt.sv_C.sv = pRTCC->ConvertSVtoEphemData(sv_Phasing);

		pRTCC->PMSTICN(lamopt, lamres);
		dV_Phasing = lamres.dV;

		sv_Phasing_apo = sv_Phasing;
		sv_Phasing_apo.V += dV_Phasing;

		//Insertion Targeting
		t_CSI = t_Insertion + dt2;

		lamopt2.T1 = pRTCC->GMTfromGET(t_Insertion);
		lamopt2.T2 = pRTCC->GMTfromGET(t_CSI);
		lamopt2.sv_C.sv = pRTCC->ConvertSVtoEphemData(sv_Phasing_apo);

		pRTCC->PMSTICN(lamopt2, lamres);
		dV_Insertion = lamres.dV;

		sv_Insertion = pRTCC->coast(sv_Phasing_apo, t_Insertion - t_Phasing);
		sv_Insertion_apo = sv_Insertion;
		sv_Insertion_apo.V += dV_Insertion;

		sv_CSI = pRTCC->coast(sv_Insertion_apo, t_CSI - t_Insertion);

		//CSI Targeting
		sv_P_CSI = pRTCC->coast(sv_P0, t_CSI - OrbMech::GETfromMJD(sv_P0.MJD, GETbase));
		OrbMech::CSIToDH(sv_CSI.R, sv_CSI.V, sv_P_CSI.R, sv_P_CSI.V, DH, OrbMech::mu_Moon, dv_CSI);
		sv_CSI_apo = sv_CSI;
		sv_CSI_apo.V = sv_CSI.V + OrbMech::ApplyHorizontalDV(sv_CSI.R, sv_CSI.V, dv_CSI);

		//CDH Targeting
		T_P = OrbMech::period(sv_CSI_apo.R, sv_CSI_apo.V, OrbMech::mu_Moon);
		t_CDH = t_CSI + T_P / 2.0;
		sv_CDH = pRTCC->coast(sv_CSI_apo, t_CDH - t_CSI);
		sv_CDH_apo = sv_CDH;
		sv_P_CDH = pRTCC->coast(sv_P_CSI, t_CDH - t_CSI);
		OrbMech::RADUP(sv_P_CDH.R, sv_P_CDH.V, sv_CDH.R, OrbMech::mu_Moon, R_P_CDH1, V_P_CDH1);
		sv_CDH_apo.V = OrbMech::CoellipticDV(sv_CDH.R, R_P_CDH1, V_P_CDH1, OrbMech::mu_Moon);

		//Find TPI time and recycle
		dt_TPI = OrbMech::findelev(pRTCC->SystemParameters.AGCEpoch, sv_CDH_apo.R, sv_CDH_apo.V, sv_P_CDH.R, sv_P_CDH.V, sv_CDH_apo.MJD, 26.6*RAD, sv_CDH_apo.gravref);
		t_TPI_apo = t_CDH + dt_TPI;
		ddt = t_TPI - t_TPI_apo;
		dt += ddt;
	}

	t_Ins = t_Insertion;
	CSI = t_CSI;

	/*
	//Debug prints
	SV sv_before, sv_after;
	MATRIX3 Rot;
	VECTOR3 DV_LVLH;
	double tig, r_apo, r_peri, h_apo, h_peri;
	char Buffer[128], Buffer2[128];

	for (int i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			sprintf(Buffer, "Phasing");
			tig = t_TIG;
			sv_before = sv_Phasing;
			sv_after = sv_Phasing_apo;
		}
		else if (i == 1)
		{
			sprintf(Buffer, "Insertion");
			tig = t_Insertion;
			sv_before = sv_Insertion;
			sv_after = sv_Insertion_apo;
		}
		else if (i == 2)
		{
			sprintf(Buffer, "CSI");
			tig = t_CSI;
			sv_before = sv_CSI;
			sv_after = sv_CSI_apo;
		}
		else if (i == 3)
		{
			sprintf(Buffer, "CDH");
			tig = t_CDH;
			sv_before = sv_CDH;
			sv_after = sv_CDH_apo;
		}

		oapiWriteLog(Buffer);

		OrbMech::format_time_HHMMSS(Buffer2, tig);
		Rot = OrbMech::LVLH_Matrix(sv_before.R, sv_before.V);
		DV_LVLH = mul(Rot, sv_after.V - sv_before.V) / 0.3048;

		sprintf(Buffer, "TIG %s DV %.1lf %.1lf %.1lf", Buffer2, DV_LVLH.x, DV_LVLH.y, DV_LVLH.z);
		oapiWriteLog(Buffer);
		OrbMech::periapo(sv_after.R, sv_after.V, OrbMech::mu_Moon, r_apo, r_peri);
		h_apo = r_apo - pRTCC->BZLAND.rad[0];
		h_peri = r_peri - pRTCC->BZLAND.rad[0];
		sprintf(Buffer, "HA %.2lf HP %.2lf", h_apo / 1852.0, h_peri / 1852.0);
		oapiWriteLog(Buffer);
	}

	oapiWriteLog("TPI");
	OrbMech::format_time_HHMMSS(Buffer2, t_TPI);
	sprintf(Buffer, "TIG %s", Buffer2);
	oapiWriteLog(Buffer);
	*/
}

void MCC_Calculations::StoreStateVector(VehicleDataBlock sv)
{
	pRTCC->calcParams.SVSTORE1 = pRTCC->ConvertEphemDatatoSV(sv.sv, sv.Weight);
}

void MCC_Calculations::StoreStateVector(SV sv)
{
	pRTCC->calcParams.SVSTORE1 = sv;
}

void MCC_Calculations::StoreStateVector(EphemerisData sv, double Weight)
{
	pRTCC->calcParams.SVSTORE1 = pRTCC->ConvertEphemDatatoSV(sv, Weight);
}

void MCC_Calculations::RestoreStateVector(VehicleDataBlock &sv)
{
	sv.sv = pRTCC->ConvertSVtoEphemData(pRTCC->calcParams.SVSTORE1);
	sv.Weight = pRTCC->calcParams.SVSTORE1.mass;
}

void MCC_Calculations::RestoreStateVector(SV &sv)
{
	sv = pRTCC->calcParams.SVSTORE1;
}