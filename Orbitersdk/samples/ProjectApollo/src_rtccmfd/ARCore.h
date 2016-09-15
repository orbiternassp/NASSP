#ifndef __ARCORE_H
#define __ARCORE_H

#include "Orbitersdk.h"
#include "MFDButtonPage.hpp"
#include "ApollomfdButtons.h"
#include "OrbMech.h"
#include "EntryCalculations.h"
#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
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

class ARCore {
public:
	ARCore(VESSEL* v);
	void lambertcalc();
	void CDHcalc();
	void OrbitAdjustCalc();
	void REFSMMATCalc();
	void LOICalc();
	void LmkCalc();
	//void EntryCalc();
	//void EntryUpdateCalc();
	void StateVectorCalc();
	bool vesselinLOS();
	void MinorCycle(double SimT, double SimDT, double mjd);
	VECTOR3 finealignLMtoCSM(VECTOR3 lmn20, VECTOR3 csmn20);

	void UplinkData();
	void UplinkData2();
	void send_agc_key(char key);
	void uplink_word(char *data);
	void P30Uplink(void);
	void EntryUplink(void);
	void EntryUpdateUplink(void);
	void REFSMMATUplink(void);
	void StateVectorUplink();
	void TimeTagUplink(void);
	void ManeuverPAD();
	void EntryPAD();
	void TPIPAD();
	void TLI_PAD();
	void MapUpdate();

	int startSubthread(int fcn);
	int subThread();
	void StartIMFDRequest();
	void StopIMFDRequest();

	// SUBTHREAD MANAGEMENT
	int subThreadMode;										// What should the subthread do?
	int subThreadStatus;									// 0 = done/not busy, 1 = busy, negative = done with error

	//OrbMech* mech;
	CoastIntegrator* coast;
	Entry* entry;
	TEI* teicalc;
	RTCC* rtcc;
	ApolloRTCCMFDData g_Data;

	//TARGETING VESSELS
	VESSEL* vessel;
	VESSEL* target;
	int targetnumber;		//Vessel index for target

	//GENERAL PARAMETERS
	double GETbase;			//Launch MJD
	double AGCEpoch;
	int mission;			//0=manual, 7 = Apollo 7, 8 = Apollo 8, 9 = Apollo 9
	OBJHANDLE gravref;		//Earth or Moon
	double P30TIG;			//Maneuver GET
	VECTOR3 dV_LVLH;		//LVLH maneuver vector
	OBJHANDLE maneuverplanet;
	int vesseltype; //0=CSM, 1=CSM/LM docked, 2 = LM, 3 = LM/CSM docked
	bool inhibUplLOS;
	double TimeTag;

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

	//CDH PAGE
	double CDHtime;	//Time of the CDH maneuver
	double CDHtime_cor;	//Corrected time of the CDH maneuver
	int CDHtimemode; //0=Fixed, 1 = Find GETI
	double DH;			//Delta Height for the CDH maneuver
	VECTOR3 CDHdeltaV;

	//ORBIT ADJUSTMENT PAGE
	double apo_desnm;		//Desired apoapsis altitude in NM
	double peri_desnm;		//Desired periapsis altitude in NM
	double incdeg;			//Desired inclination in degrees
	double SPSGET;			//Maneuver GET
	VECTOR3 OrbAdjDVX;		//LVLH maneuver vector

	//REFSMMAT PAGE
	double REFSMMATTime;
	MATRIX3 REFSMMAT;
	int REFSMMATopt; //Displayed REFSMMAT page: 0 = P30 Maneuver, 1 = P30 Retro, 2= LVLH, 3= Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC, 7 = LOI-2
	int REFSMMAToct[20];
	int REFSMMATcur; //Currently saved REFSMMAT: 0 = P30 Maneuver, 1 = P30 Retro, 2= LVLH, 3= Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC, 7 = LOI-2
	int REFSMMATupl; //0 = Desired REFSMMAT, 1 = REFSMMAT
	double LSLat, LSLng;
	bool REFSMMATdirect;

	//ENTY PAGE	
	int entrycritical; //0 = Fuel critical, 1 = time critical, 2 = Abort
	bool entrynominal; //0 = minimum DV, 1 = 31.7° line
	double EntryTIG;
	double EntryLat;
	double EntryLng;
	double EntryAng, EntryAngcor;
	double EntryTIGcor;
	double EntryLatcor;
	double EntryLngcor;
	VECTOR3 Entry_DV;
	int entrycalcmode; //0=LEO mode with angle and longitude, 1=Entry Prediction, 2=P37 Block Data, 3 = TEI
	int entrycalcstate;
	double entryrange;
	double P37GET400K;
	bool entrylongmanual; //0 = landing zone, 1 = manual longitude input
	int landingzone; //0 = Mid Pacific, 1 = East Pacific, 2 = Atlantic Ocean, 3 = Indian Ocean, 4 = West Pacific
	int entryprecision; //0 = conic, 1 = precision, 2 = PeA=-30 solution
	int returnspeed; //0 = slow return, 1 = normal return, 2 = fast return
	int TEItype;	//0 = TEI, 1 = Flyby, 2 = PC+2
	bool TEIfail;

	//STATE VECTOR PAGE
	bool SVSlot;
	VECTOR3 J2000Pos, J2000Vel;
	double J2000GET;
	VESSEL* svtarget;
	int svtargetnumber;
	bool svtimemode; //0 = Now, 1 = GET

	//MANEUVER PAD PAGE
	AP11MNV manpad;
	char GDCset[64];
	bool HeadsUp;
	VECTOR3 TPIPAD_dV_LOS, TPIPAD_BT;
	double TPIPAD_dH, TPIPAD_R, TPIPAD_Rdot, TPIPAD_ELmin5, TPIPAD_AZ, TPIPAD_ddH;
	int manpadopt; //0 = Maneuver PAD, 1 = TPI PAD, 2 = TLI PAD
	int ManPADSPS; //0=SPS, 1=RCS +X, 2=RCS -X
	double sxtstardtime;
	TLIPAD tlipad;

	///ENTRY PAD PAGE
	AP11ENT lunarentrypad;
	AP7ENT earthentrypad;
	int entrypadopt; //0 = Earth Entry Update, 1 = Lunar Entry
	double EntryRTGO;
	bool EntryPADdirect;

	//MAP UPDATE PAGE
	double LOSGET, AOSGET, SSGET, SRGET, PMGET, GSAOSGET, GSLOSGET;
	int mappage, mapgs;

	//LOI PAGE
	int LOImaneuver; //0 = Last MCC, 1 = LOI-1 (w/ MCC), 2 = LOI-1 (w/o MCC), 3 = LOI-2, 4 = TLI
	double LOIGET, LOIPeriGET, LOILat, LOILng;
	double LOIapo, LOIperi, LOIinc;
	VECTOR3 TLCC_dV_LVLH, LOI_dV_LVLH;
	double TLCC_TIG, LOI_TIG;
	VECTOR3 R_TLI, V_TLI;

	//LANDMARK TRACKING PAGE
	double LmkLat, LmkLng;
	double LmkTime;
	double LmkT1, LmkT2;
	double LmkRange;
	double LmkN89Lat, LmkN89Alt;
private:
	//VECTOR3 RA2, VA2, RP2, VP2;
};




#endif // !__ARCORE_H