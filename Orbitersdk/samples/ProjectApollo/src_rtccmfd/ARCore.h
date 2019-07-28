#ifndef __ARCORE_H
#define __ARCORE_H

#include "Orbitersdk.h"
#include "MFDButtonPage.hpp"
#include "ApollomfdButtons.h"
#include "OrbMech.h"
#include "EntryCalculations.h"
#include "soundlib.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "mcc.h"
#include "rtcc.h"
#include <queue>

struct ApolloRTCCMFDData {  // global data storage
	int connStatus;
	int emem[24];
	int uplinkState;
	int uplinkLEM;
	IMFD_BURN_DATA burnData;
	std::queue<unsigned char> uplinkBuffer;
	double uplinkBufferSimt;
	bool isRequesting;
	Saturn *progVessel;
};

class AR_GCore
{
public:
	AR_GCore(VESSEL* v);

	MPTable mptable;
	bool MissionPlanningActive;
	double GETbase;			//Launch MJD
	int mission;				//0=manual, 7 = Apollo 7, 8 = Apollo 8, 9 = Apollo 9, etc.
	double LSLat, LSLng, LSAlt;	//Landing Site coordinates
	double t_Land;				//Time of landing
	double LOIazi;
	double TLCCFreeReturnEMPLat, TLCCNonFreeReturnEMPLat;
	//Initial guess of pericynthion GET
	double TLCCPeriGET;
	double LOIapo, LOIperi;
	int LOIEllipseRotation;	//0 = Choose the lowest DV solution, 1 = solution 1, 2 = solution 2
	double TLCCFlybyPeriAlt, TLCCLAHPeriAlt;
	double TLCCNodeLat, TLCCNodeLng, TLCCNodeAlt, TLCCNodeGET;
	int DOI_N;							//Number of revolutions between DOI and PDI
	int DOI_option;						//0 = DOI from circular orbit, 1 = DOI as LOI-2
	double DOI_PeriAng;					//Angle from landing site to PDI position
	double DOI_alt;						//perilune altitude above landing site
	double RTEMaxReturnInclination;
	double RTERangeOverrideNM;
};

class ARCore {
public:
	ARCore(VESSEL* v, AR_GCore* gcin);
	~ARCore();
	void lambertcalc();
	void CDHcalc();
	void GPMPCalc();
	void REFSMMATCalc();
	void SkylabCalc();
	void DOICalc();
	void PCCalc();
	void LunarLiftoffCalc();
	void LOICalc();
	void LmkCalc();
	void MoonRTECalc();
	void EntryCalc();
	void DeorbitCalc();
	void TLCCCalc();
	void EntryUpdateCalc();
	void StateVectorCalc();
	void AGSStateVectorCalc();
	void LandingSiteUpdate();
	void LSUplinkCalc();
	void LandingSiteUplink();
	void VecPointCalc();
	void TerrainModelCalc();
	void DKICalc();
	void LAPCalc();
	void DAPPADCalc();
	void AscentPADCalc();
	void PDAPCalc();
	void UpdateFIDOOrbitDigitals();
	void CycleFIDOOrbitDigitals();
	void FIDOOrbitDigitalsCalculateLongitude();
	void FIDOOrbitDigitalsCalculateGETL();
	void FIDOOrbitDigitalsApoPeriRequest();
	void UpdateSpaceDigitals();
	void CycleSpaceDigitals();
	void SpaceDigitalsGET();
	void CycleNextStationContactsDisplay();
	void CyclePredictedSiteAcquisitionDisplay();
	void CalculatePredictedSiteAcquisitionDisplay();
	bool vesselinLOS();
	void MinorCycle(double SimT, double SimDT, double mjd);

	void UplinkData();
	void UplinkData2();
	void send_agc_key(char key);
	void uplink_word(char *data);
	void P30Uplink(void);
	void EntryUplink(void);
	void EntryUpdateUplink(void);
	void REFSMMATUplink(void);
	void StateVectorUplink();
	void TLANDUplink(void);
	void EMPP99Uplink(int i);
	void ManeuverPAD();
	void EntryPAD();
	void TPIPAD();
	void TLI_PAD();
	void PDI_PAD();
	void MapUpdate();
	void NavCheckPAD();
	void AP11AbortCoefUplink();
	int REFSMMATOctalAddress();
	int REFSMMATUplinkAddress();
	void DetermineGMPCode();

	int startSubthread(int fcn);
	int subThread();
	void StartIMFDRequest();
	void StopIMFDRequest();

	//EPHEM PROGRAM
	void GenerateAGCEphemeris();
	int agcCelBody_RH(CELBODY *Cel, double mjd, int Flags, VECTOR3 *Pos = NULL, VECTOR3 *Vel = NULL);
	int agcCelBody_LH(CELBODY *Cel, double mjd, int Flags, VECTOR3 *Pos = NULL, VECTOR3 *Vel = NULL);
	void AGCEphemeris(double T0, double Epoch, double TEphem0);
	void AGCCorrectionVectors(double mjd_launch, double t_land, int mission, bool isCMC);
	void GenerateAGCCorrectionVectors();

	// SUBTHREAD MANAGEMENT
	HANDLE hThread;
	int subThreadMode;										// What should the subthread do?
	int subThreadStatus;									// 0 = done/not busy, 1 = busy, negative = done with error

	RTCC* rtcc;
	ApolloRTCCMFDData g_Data;

	//TARGETING VESSELS
	VESSEL* vessel;
	VESSEL* target;
	int targetnumber;		//Vessel index for target

	//GENERAL PARAMETERS
	double AGCEpoch;
	double P30TIG;				//Maneuver GET
	VECTOR3 dV_LVLH;			//LVLH maneuver vector
	int vesseltype;				//0=CSM, 1=CSM/LM docked, 2 = LM, 3 = LM/CSM docked
	bool lemdescentstage;		//0 = ascent stage, 1 = descent stage
	bool inhibUplLOS;
	bool PADSolGood;
	int enginetype;				// 0 = RCS, 1 = SPS or DPS or APS
	int directiontype;			// 0 = +X, 1 = -X (RCS only)
	double t_TPI;				// Generally used TPI time

	//LAMBERT PAGE
	double T1;				//Time of the Lambert targeted maneuver
	double T2;				//Arrival time for Lambert Targeting
	double lambertelev;		//Elevation of target for T1 calculation
	int N;					//Number of revolutions for Lambert Targeting
	VECTOR3 LambertdeltaV;	//LVLH maneuver vector
	int lambertopt;			//0 = spherical, 1 = non-spherical
	VECTOR3 offvec;			//Lambert offset vector
	double angdeg;			//Phase angle for target offset
	bool lambertmultiaxis; //0 = x-axis only, 1 = multi-axis maneuver
	int twoimpulsemode;		//0 = General, 1 = NCC/NSR, 2 = TPI/TPF
	double TwoImpulse_TPI;	//TPI time calculated by the NCC/NSR option
	double TwoImpulse_PhaseAngle;	//Phase angle of chaser relative to target at T2 time
	int lambertElevOpt;		//0 = T1 on time, 1 = search for elevation angle
	int lambertTPFOpt;		//0 = T2 on time, 1 = use DT from T1, 2 = use travel angle
	double lambertDT;		//DT from T1 to T2
	double lambertWT;		//travel angle of passive vehicle between T1 and T2

	//DOCKING INITIATION
	double DKI_TIG;		//Impulsive time of ignition
	int DKI_Profile;	//0 = Four-impulse: Phasing/CSI/CDH/TPI, 1 = Six-Impulse: Phasing/Boost/HAM/CSI/CDH/TPI, 2 = Four-impulse rescue: Height/CSI/CDH/TPI, 3 = Calculate TPI time only
	int DKI_TPI_Mode;	//0 = TPI on time, 1 = TPI at orbital midnight, 2 = TPI at X minutes before sunrise
	bool DKI_Maneuver_Line;	//false = define relative times, true = 0.5 revolutions between maneuvers
	bool DKI_Radial_DV;	//false = horizontal maneuver, true = 50 ft/s radial component
	double DKI_dt_TPI_sunrise;
	double DKI_dt_PBH;	//Delta time between phasing and boost/CSI
	double DKI_dt_BHAM;	//Delta time between boost and HAM
	double DKI_dt_HAMH;	//Delta time between HAM and CSI
	int DKI_N_HC;		//Half revolutions between CSI and CDH
	int DKI_N_PB;		//Number of half revs between Phasing and Boost/Height
	double t_TPIguess;
	DKIResults dkiresult;

	//CONCENTRIC RENDEZVOUS PAGE
	int SPQMode;	//0 = CSI, 1 = CDH
	double CSItime;	//Time of the CSI maneuver
	double CDHtime;	//Time of the CDH maneuver
	double SPQTIG;	//Time of ignition for concentric rendezvous maneuver
	int CDHtimemode; //CSI: 0 = fixed TIG at TPI, 1 = fixed DH at CDH. CDH: 0=Fixed, 1 = Find GETI
	double DH;			//Delta Height for the CDH maneuver
	VECTOR3 SPQDeltaV;
	SPQResults spqresults;

	//ORBIT ADJUSTMENT PAGE
	int GMPManeuverCode; //Maneuver code
	bool OrbAdjAltRef;	//0 = use mean radius, 1 = use launchpad or landing site radius
	double GMPApogeeHeight;		//Desired apoapsis height
	double GMPPerigeeHeight;	//Desired periapsis height
	double GMPWedgeAngle;
	double GMPManeuverHeight;
	double GMPManeuverLongitude;
	double GMPHeightChange;
	double GMPNodeShiftAngle;
	double GMPDeltaVInput;
	double GMPPitch;
	double GMPYaw;
	double GMPApseLineRotAngle;
	int GMPRevs;
	double SPSGET;		//Maneuver GET
	VECTOR3 OrbAdjDVX;	//LVLH maneuver vector
	//0 = Apogee
	//1 = Equatorial crossing
	//2 = Perigee
	//3 = Longitude
	//4 = Height
	//5 = Time
	//6 = Optimum
	int GMPManeuverPoint;
	//0 = Plane Change
	//1 = Circularization
	//2 = Height Change
	//3 = Node Shift
	//4 = Apogee and perigee change
	//5 = Input maneuver
	//6 = Combination apogee/perigee change and node shift
	//7 = Shift line-of-apsides
	//8 = Combination height maneuver and plane change
	//9 = Combination circularization and plane change
	//10 = Combination circularization and node shift
	//11 = Combination height maneuver and node shift
	//12 = Combination apogee/perigee change and line-of-apsides shift
	int GMPManeuverType;
	GPMPRESULTS GMPResults;

	//REFSMMAT PAGE
	double REFSMMATTime;
	MATRIX3 REFSMMAT;
	int REFSMMATopt; //Displayed REFSMMAT page: 0 = P30 Maneuver, 1 = P30 Retro, 2 = LVLH, 3 = Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC, 7 = Attitude, 8 = LS during TLC
	int REFSMMAToct[20];
	int REFSMMATcur; //Currently saved REFSMMAT
	int REFSMMATupl; //0 = Desired REFSMMAT, 1 = REFSMMAT
	bool REFSMMATHeadsUp;

	//ENTY PAGE	
	int entrycritical; //1 = Midcourse, 2 = Abort, 3 = Corridor Control
	bool entrynominal; //0 = minimum DV, 1 = 31.7° line
	double EntryTIG;
	double EntryLat;
	double EntryLng;
	double EntryAng, EntryAngcor;
	double EntryTIGcor;
	double EntryLatcor;
	double EntryLngcor;
	VECTOR3 Entry_DV;
	double entryrange;
	double P37GET400K;
	bool entrylongmanual; //0 = landing zone, 1 = manual longitude input
	int landingzone; //0 = Mid Pacific, 1 = East Pacific, 2 = Atlantic Ocean, 3 = Indian Ocean, 4 = West Pacific
	int entryprecision; //0 = conic, 1 = precision, 2 = PeA=-30 solution
	double RTEReentryTime;
	double FlybyPeriAlt;
	double EntryDesiredInclination;
	int RTECalcMode; // 0 = ATP Tradeoff, 1 = ATP Search, 2 = ATP Discrete, 3 = UA Search, 4 = UA Discrete
	double RTEReturnInclination;
	double RTEMaxReentrySpeed;

	//STATE VECTOR PAGE
	bool SVSlot;
	SV UplinkSV;
	double SVDesiredGET;
	VESSEL* svtarget;
	int svtargetnumber;
	double AGSEpochTime;
	VECTOR3 AGSPositionVector, AGSVelocityVector;
	double AGSKFactor;
	AP11AGSSVPAD agssvpad;
	int SVOctals[021];
	VECTOR3 RLSUplink;
	int RLSOctals[010];

	//MANEUVER PAD PAGE
	AP11MNV manpad;
	AP11LMMNV lmmanpad;
	char GDCset[64];
	bool HeadsUp;
	VECTOR3 TPIPAD_dV_LOS, TPIPAD_BT;
	double TPIPAD_dH, TPIPAD_R, TPIPAD_Rdot, TPIPAD_ELmin5, TPIPAD_AZ, TPIPAD_ddH;
	int manpadopt; //0 = Maneuver PAD, 1 = TPI PAD, 2 = TLI PAD
	double sxtstardtime;
	TLIPAD tlipad;
	AP11PDIPAD pdipad;

	///ENTRY PAD PAGE
	AP11ENT lunarentrypad;
	AP7ENT earthentrypad;
	int entrypadopt; //0 = Earth Entry Update, 1 = Lunar Entry
	double EntryRTGO;

	//MAP UPDATE PAGE
	AP10MAPUPDATE mapupdate;
	double GSAOSGET, GSLOSGET;
	int mappage, mapgs;
	double mapUpdateGET;

	//TLCC PAGE

	//0 = TLI (nodal), 1 = TLI (free return), 2 = XYZ and T (Nodal) Targeting, 3 = FR BAP Fixed LPO, 4 = FR BAP Free LPO
	//5 = Non Free BAP Fixed LPO, 6 = Non Free BAP Free LPO, 7 = Circumlunar free-return flyby, specified H_PC and phi_PC
	int TLCCmaneuver;
	VECTOR3 TLCC_dV_LVLH;
	//Corrected time of pericynthion
	double TLCCPeriGETcor;
	//Initial guess and corrected TIG
	double TLCC_GET, TLCC_TIG;
	double TLCCReentryGET, TLCCFRIncl, TLCCEMPLatcor;
	double TLCCFRLat, TLCCFRLng;
	VECTOR3 R_TLI, V_TLI;
	bool TLCCSolGood;
	bool TLCCAscendingNode;
	double TLCCFRDesiredInclination;
	int TLCCIterationStep;
	double TLCCRev2MeridianGET;
	double TLCCPostDOIPeriAlt, TLCCPostDOIApoAlt;

	//LOI PAGE
	int LOImaneuver; //0 = LOI-1, 1 = LOI-2
	int LOIOption;	//0 = Fixed LPO, 1 = LOI at Peri
	double LOI2Alt;
	VECTOR3 LOI_dV_LVLH;
	double LOI_TIG;
	double LOI2_EarliestGET;

	//LANDMARK TRACKING PAGE
	AP11LMARKTRKPAD landmarkpad;
	double LmkLat, LmkLng;
	double LmkTime;

	//VECPOINT PAGE
	int VECoption;		//0 = Point SC at body, 1 = Open hatch thermal control, 2 = Point AOT with CSM
	int VECdirection;	//0 = +X, 1 = -X, 2 = +Y,3 = -Y,4 = +Z, 5 = -Z
	OBJHANDLE VECbody;	//handle for the desired body
	VECTOR3 VECangles;	//IMU angles

	//DOI Page
	double DOIGET;						//Initial guess for the DOI TIG
	double DOI_TIG;						//Integrated DOI TIG
	VECTOR3 DOI_dV_LVLH;				//Integrated DV Vector
	double DOI_t_PDI, DOI_CR;			//Time of PDI, cross range at PDI

	//Skylab Page
	int Skylabmaneuver;					//0 = Presettings, 1 = NC1, 2 = NC2, 3 = NCC, 4 = NSR, 5 = TPI, 6 = TPM, 7 = NPC
	bool Skylab_NPCOption;				//0 = NC1 or NC2 with out-of-plane component, setting up a NPC maneuver 90° later
	bool Skylab_PCManeuver;				//0 = NC1 is setting up NPC, 1 = NC2 is setting up NPC
	double SkylabTPIGuess;
	double Skylab_n_C;
	double SkylabDH1;					//Delta Height at NCC
	double SkylabDH2;					//Delta Height at NSR
	double Skylab_E_L;
	bool SkylabSolGood;
	VECTOR3 Skylab_dV_NSR, Skylab_dV_NCC;//, Skylab_dV_NPC;
	double Skylab_dH_NC2, Skylab_dv_NC2, Skylab_dv_NCC;
	double Skylab_t_NC1, Skylab_t_NC2, Skylab_t_NCC, Skylab_t_NSR, Skylab_dt_TPM; //Skylab_t_NPC

	//PC Page
	double PCAlignGET;		//time when the orbit is aligned with the landing site
	bool PClanded;			//0 = use lat/lng/alt to calculate landing site, 1 = target vessel on the surface
	VECTOR3 PC_dV_LVLH;
	double PCEarliestGET;	//Initial guess for the PC TIG
	double PC_TIG;			//Corrected PC TIG


	//Terrain Model
	double TMLat, TMLng, TMAzi, TMDistance, TMStepSize, TMAlt;

	//Lunar Liftoff Time Prediction
	LunarLiftoffResults LunarLiftoffRes;
	int LunarLiftoffTimeOption;	//0 = Concentric Profile, 1 = Direct Profile, 2 = Time Critical Direct Profile
	double DT_Ins_TPI;			//Fixed time from insertion to TPI for direct profile
	double t_Liftoff_guess;		//Threshold time for lunar liftoff
	bool LunarLiftoffInsVelInput;	//0 = Calculate velocity internally, 1 = use input velocity

	//Lunar Ascent Processor
	double LAP_Theta;			//Angle travelled between liftoff and insertion
	double LAP_DT;				//Ascent burntime (liftoff to insertion)
	SV LAP_SV_Insertion;

	//LM Ascent PAD
	AP11LMASCPAD lmascentpad;

	//Powered Descent Abort Program
	int PDAPEngine;	//0 = DPS/APS, 1 = APS
	bool PDAPTwoSegment;	//false = One Segment (Luminary099, FP6), true = Two Segment (Luminary116 and later, FP7 and later)
	double PDAPABTCOF[8];	//Luminary099 abort coefficients
	double DEDA224, DEDA225, DEDA226;
	int DEDA227;
	double PDAP_J1, PDAP_K1, PDAP_J2, PDAP_K2, PDAP_Theta_LIM, PDAP_R_amin;

	//Erasable Memory Programs
	int EMPUplinkType;	// 0 = P99
	int EMPUplinkNumber;

	//NAV CHECK PAGE
	AP7NAV navcheckpad;

	//DAP PAD PAGE
	AP10DAPDATA DAP_PAD;

	//LVDC PAGE
	double LVDCLaunchAzimuth;

	//AGC EPHEMERIS
	int AGCEphemOption;	//0 = AGC ephemerides, 1 = AGC precession/nutation/libration correction vectors
	double AGCEphemBRCSEpoch;
	double AGCEphemTEphemZero;
	double AGCEphemTIMEM0;
	double AGCEphemTEPHEM;
	double AGCEphemTLAND;
	int AGCEphemMission;
	bool AGCEphemIsCMC;

	//FIDO ORBIT DIGITALS
	FIDOOrbitDigitals fidoorbit;
	SV fidoorbitsv;

	//SPACE DIGITALS
	SpaceDigitals spacedigit;
	SV spacedigitalssv;

	//NEXT STATION CONTACT DISPLAY
	OrbitStationContactsTable orbitstatconttable;
	NextStationContactsTable nextstatconttable;
	PredictedSiteAcquisitionTable predsiteacqtable;
	bool nextstatcont_lunar; //Only use stations with lunar capability
	double predsiteacq_GET;
	double predsiteacq_DT;

	protected:
		int GetPowEngType();

private:
	AR_GCore* GC;
};




#endif // !__ARCORE_H