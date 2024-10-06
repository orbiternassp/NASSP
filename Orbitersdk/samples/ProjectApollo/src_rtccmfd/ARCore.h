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
#include "CSMcomputer.h"
#include "saturn.h"
#include "mcc.h"
#include "rtcc.h"
#include "LunarTargetingProgram.h"
#include "thread.h"
#include "RTCCDisplayFormatting.h"
#include <queue>

struct ApolloRTCCMFDData {  // global data storage
	int connStatus;
	int emem[24];
	int uplinkState;
	std::queue<unsigned char> uplinkBuffer;
	double uplinkBufferSimt;
};

class AR_GCore
{
public:
	AR_GCore(VESSEL* v);
	~AR_GCore();

	void SetMissionSpecificParameters(int mission);
	void MPTMassUpdate();
	int MPTTrajectoryUpdate(VESSEL *ves, bool csm);

	bool AGOP_CSM_REFSMMAT_Required();
	bool AGOP_LM_REFSMMAT_Required();

	bool MissionPlanningActive;

	int mptInitError;

	double REFSMMAT_PTC_MJD;

	RTCC* rtcc;

	//MANEUVER PAD PAGE
	AP11MNV manpad;
	AP11LMMNV lmmanpad;
	AP7TPI TPI_PAD;
	TLIPAD tlipad;
	AP11PDIPAD pdipad;

	//ENTRY PAD PAGE
	AP11ENT lunarentrypad;
	AP7ENT earthentrypad;
	int entrypadopt; //0 = Earth Entry Update, 1 = Lunar Entry
	bool EntryPADSxtStarCheckAttOpt; //true = sextant star attitude check at entry attitude, false = sextant star check at horizon check attitude

	//LANDMARK TRACKING PAGE
	AP11LMARKTRKPAD landmarkpad;
	double LmkLat, LmkLng;
	double LmkTime;
	double LmkElevation;

	//APOLLO GENERALIZED OPTICS PROGRAM
	int AGOP_Page;
	int AGOP_Option;
	int AGOP_Mode;
	int AGOP_AdditionalOption;
	double AGOP_StartTime;
	double AGOP_StopTime;
	double AGOP_TimeStep; //in minutes
	int AGOP_CSM_REFSMMAT;
	int AGOP_LM_REFSMMAT;
	int AGOP_Stars[2];
	double AGOP_Lat, AGOP_Lng, AGOP_Alt;
	VECTOR3 AGOP_Attitudes[2];
	bool AGOP_AttIsCSM;
	bool AGOP_HeadsUp;
	double AGOP_AntennaPitch, AGOP_AntennaYaw;
	int AGOP_Instrument;
	double AGOP_InstrumentAngles1[2];
	double AGOP_InstrumentAngles2[2];
	bool AGOP_LMCOASAxis;
	int AGOP_LMAOTDetent;
	std::vector<std::string> AGOP_Output;
	std::string AGOP_Error;
	MATRIX3 AGOP_REFSMMAT;
	int AGOP_REFSMMAT_Vehicle;

	//MOCR DISPLAY
	void DFLBackgroundSlide(oapi::Sketchpad *skp, DWORD W, DWORD H, unsigned display);

protected:
	const rtcc::RTCCBackgroundSlides BackgroundSlides;
};

class ARCore {
public:
	ARCore(VESSEL* v, AR_GCore* gcin);
	~ARCore();
	void LunarLaunchTargetingCalc();
	void LDPPalc();
	void LunarLiftoffCalc();
	void LOICalc();
	void LmkCalc();
	void EntryCalc();
	void DeorbitCalc();
	void TLCCCalc();
	void EntryUpdateCalc();
	void StateVectorCalc(int type);
	void AGSStateVectorCalc(bool IsCSM);
	void LandingSiteUpdate();
	void CSMLSUplinkCalc();
	void LMLSUplinkCalc();
	void CSMLandingSiteUplink();
	void LMLandingSiteUplink();
	void VecPointCalc(bool IsCSM);
	void TerrainModelCalc();
	void LAPCalc();
	void DAPPADCalc(bool IsCSM);
	void AscentPADCalc();
	void PDAPCalc();
	void CycleFIDOOrbitDigitals1();
	void CycleFIDOOrbitDigitals2();
	void CycleSpaceDigitals();
	void CycleVectorPanelSummary();
	void SpaceDigitalsMSKRequest();
	void CycleNextStationContactsDisplay();
	void RecoveryTargetSelectionCalc();
	void RTETradeoffDisplayCalc();
	void GetAGSKFactor();
	void GeneralMEDRequest();
	void SkylabSaturnIBLaunchCalc();
	void SkylabSaturnIBLaunchUplink();
	void TransferTIToMPT();
	void TransferSPQToMPT();
	void TransferDKIToMPT();
	void TransferDescentPlanToMPT();
	void TransferPoweredDescentToMPT();
	void TransferPoweredAscentToMPT();
	void TransferGPMToMPT();
	void MPTDirectInputCalc();
	void MPTTLIDirectInput();
	void AbortScanTableCalc();
	void TransferLOIorMCCtoMPT();
	void TransferRTEToMPT();
	void SLVNavigationUpdateCalc();
	void SLVNavigationUpdateUplink();
	void UpdateGRRTime(VESSEL *v);
	void PerigeeAdjustCalc();
	void MinorCycle(double SimT, double SimDT, double mjd);

	void UplinkData(bool isCSM);
	void UplinkData2(bool isCSM);
	void UplinkDataV70V73(bool v70, bool isCSM);
	void send_agc_key(char key, bool isCSM);
	void uplink_word(char *data, bool isCSM);
	void P30UplinkCalc(bool isCSM);
	void P30Uplink(bool isCSM);
	void RetrofireEXDVUplinkCalc(char source, char column);
	void RetrofireEXDVUplink();
	void EntryUplinkCalc();
	void EntryUpdateUplink(void);
	void REFSMMATUplink(bool isCSM);
	void StateVectorUplink(int type);
	void TLANDUplinkCalc(void);
	void TLANDUplink(void);
	void AGCClockIncrementUplink(bool csm);
	void AGCLiftoffTimeIncrementUplink(bool csm);
	void ErasableMemoryFileRead();
	void ErasableMemoryFileLoad(int blocknum);
	void ErasableMemoryUpdateUplink(int blocknum);

	void ManeuverPAD(bool IsCSM);
	void EntryPAD();
	void TPIPAD();
	void TLI_PAD();
	void PDI_PAD();
	void MapUpdate(bool IsCSM);
	void NavCheckPAD(bool IsCSM);
	void AP11AbortCoefUplink();
	void AP12AbortCoefUplink();
	void DetermineGMPCode();
	void NodeConvCalc();
	void SendNodeToSFP();
	void CalculateTPITime();
	agc_t *GetAGCPointer(bool cmc) const;
	void GetStateVectorFromAGC(bool csm, bool cmc);
	void GetStateVectorFromIU();
	void GetStateVectorsFromAGS();
	void VectorCompareDisplayCalc();
	void GenerateSpaceDigitalsNoMPT();
	void LUNTARCalc();
	void TLIProcessorCalc();
	void SaturnVTLITargetUplink();
	int GetVesselParameters(bool IsCSM, int docked, int Thruster, int &Config, int &TVC, double &CSMMass, double &LMMass);
	int menuCalculateIMUComparison(bool IsCSM);
	void menuCalculateIMUParkingAngles(agc_t* agc);

	int startSubthread(int fcn, bool IsCSM = true);
	int subThread();

	// SUBTHREAD MANAGEMENT
	KillableWorker subThreadWorker;
	int subThreadMode;										// What should the subthread do?
	std::atomic<ThreadStatus> subThreadStatus;
	bool IsCSMCalculation;									// Vessel selected for calculation

	ApolloRTCCMFDData g_Data;

	//GENERAL PARAMETERS
	double P30TIG;				//Maneuver GET
	VECTOR3 dV_LVLH;			//LVLH maneuver vector
	bool vesselisdocked;		// false = undocked, true = docked
	bool lemdescentstage;		//0 = ascent stage, 1 = descent stage
	bool PADSolGood;
	int manpadenginetype;
	double t_TPI;				// Generally used TPI time
	int mptinitmode;			//0 = MED M49, 1 = MED M50, 2 = MED M51, 3 = MED M55

	//DOCKING INITIATION
	int TPI_Mode;
	double dt_TPI_sunrise;
	double t_TPIguess;

	//CONCENTRIC RENDEZVOUS PAGE
	int SPQMode;	//0 = CSI on time, 1 = CDH, 2 = optimum CSI
	double CSItime;	//Time of the CSI maneuver
	double CDHtime;	//Time of the CDH maneuver
	double SPQTIG;	//Time of ignition for concentric rendezvous maneuver
	int CDHtimemode; //CSI: 0 = fixed TIG at TPI, 1 = fixed DH at CDH. CDH: 0=Fixed, 1 = Find GETI
	VECTOR3 SPQDeltaV;

	//ORBIT ADJUSTMENT PAGE
	int GMPManeuverCode; //Maneuver code
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

	//REFSMMAT PAGE
	double REFSMMAT_LVLH_Time;
	int REFSMMATopt; //Displayed REFSMMAT page: 0 = P30 Maneuver, 1 = P30 Retro, 2 = LVLH, 3 = Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC, 7 = Attitude, 8 = LS during TLC
	int REFSMMATcur; //Currently saved REFSMMAT
	bool REFSMMATHeadsUp;

	//ENTRY PAGE
	double entryrange;
	int landingzone; //0 = Mid Pacific, 1 = East Pacific, 2 = Atlantic Ocean, 3 = Indian Ocean, 4 = West Pacific
	int entryprecision; //0 = conic, 1 = precision, 2 = PeA=-30 solution
	double RTEReentryTime; //Desired landing time
	int RTECalcMode; // 0 = ATP Tradeoff, 1 = ATP Search, 2 = ATP Discrete, 3 = UA Search, 4 = UA Discrete
	int RTETradeoffMode; //0 = Near-Earth (F70), 1 = Remote-Earth (F71)
	int RTEASTType; //75 = unspecified, 76 = specific site, 77 = lunar search

	//STATE VECTOR PAGE
	double SVDesiredGET;

	//AGS STATE VECTOR
	double AGSEpochTime;
	VECTOR3 AGSPositionVector, AGSVelocityVector;
	AP11AGSSVPAD agssvpad;

	//MANEUVER PAD PAGE
	bool HeadsUp;
	int manpadopt; //0 = CSM Maneuver PAD, 1 = LM Maneuver PAD, 2 = TPI PAD, 3 = TLI PAD, 4 = PDI PAD
	double sxtstardtime;
	double manpad_ullage_dt;
	bool manpad_ullage_opt; //true = 4 jets, false = 2 jets
	int ManPADMPT; //1 = CSM, 3 = LEM
	int ManPADMPTManeuver; //1-15
	bool TLIPAD_StudyAid; //False = nominal TLI, true = study aid

	//MAP UPDATE PAGE
	AP10MAPUPDATE mapupdate;
	double GSAOSGET, GSLOSGET;
	int mappage, mapgs;
	double mapUpdateGET;
	bool mapUpdatePM; //true = 180°, false = 150°

	//TLCC PAGE
	int TLCCSolGood;

	//VECPOINT PAGE
	int VECoption;		//0 = Point SC at body, 1 = Open hatch thermal control
	int VECdirection;	//0 = +X, 1 = -X, 2 = Optics, 3 = SIM Bay, 4 = Selectable
	VECTOR3 VECBodyVector; //Yaw, pitch for option 7 and Omicron
	OBJHANDLE VECbody;	//handle for the desired body
	VECTOR3 VECangles;	//IMU angles

	//Terrain Model
	double TMLat, TMLng, TMAzi, TMDistance, TMStepSize, TMAlt;

	//LM Ascent PAD
	AP11LMASCPAD lmascentpad;
	double t_LunarLiftoff;
	int AscentPADVersion; //0 = Apollo 11-13, 1 = Apollo 14-17
	double LAP_Phase, LAP_CR;

	//Powered Descent Abort Program
	int PDAPEngine;	//0 = DPS/APS, 1 = APS
	bool PDAPTwoSegment;	//false = One Segment (Luminary099, FP6), true = Two Segment (Luminary116 and later, FP7 and later)
	double PDAPABTCOF[8];	//Luminary099 abort coefficients
	double DEDA224, DEDA225, DEDA226;
	int DEDA227;
	double PDAP_J1, PDAP_K1, PDAP_J2, PDAP_K2, PDAP_Theta_LIM, PDAP_R_amin;

	//Erasable Memory Programs
	std::string EMPFile;
	int EMPUplinkNumber, EMPUplinkMaxNumber;
	std::string EMPDescription, EMPRope, EMPErrorMessage;

	//NAV CHECK PAGE
	AP7NAV navcheckpad;

	//DAP PAD PAGE
	AP10DAPDATA DAP_PAD;

	//LVDC PAGE
	double LVDCLaunchAzimuth;

	//NODAL TARGET CONVERSION
	bool NodeConvOpt; //false = EMP to selenographc, true = selenographic to EMP
	double NodeConvLat;
	double NodeConvLng;
	double NodeConvGET;
	double NodeConvHeight;
	double NodeConvResLat;
	double NodeConvResLng;

	//SPACE DIGITALS
	int SpaceDigitalsOption;
	double SpaceDigitalsGET;

	//SATURN IB LAUNCH TARGETING
	VESSEL* Rendezvous_Target; //Target vessel in orbit

	//UPLINK
	double AGCClockTime[2];
	double RTCCClockTime[2];
	double DeltaClockTime[2];
	double DesiredRTCCLiftoffTime[2];

	VESSEL *iuvessel;
	int iuUplinkResult; //0 = no uplink, 1 = uplink accepted, 2 = vessel has no IU, 3 = uplink rejected, 4 = No targeting parameters

	//LUNAR TARGETING PROGRAM
	double LUNTAR_lat;
	double LUNTAR_lng;
	double LUNTAR_bt_guess;
	double LUNTAR_pitch_guess;
	double LUNTAR_yaw_guess;
	double LUNTAR_TIG;
	LunarTargetingProgramOutput LUNTAR_Output;

	//DEBUG
	VECTOR3 DebugIMUTorquingAngles;

	//IMU PARKING ANGLES
	unsigned int GravVec[6];
	VECTOR3 IMUParkingAngles;

private:

	AR_GCore* GC;
};




#endif // !__ARCORE_H