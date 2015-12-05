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

enum apves { CSM, LM };

class ARCore {
public:
	ARCore(VESSEL* v);
	void lambertcalc();
	void CDHcalc();
	void OrbitAdjustCalc();
	void REFSMMATCalc();
	void LOICalc();
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
	RTCC* rtcc;
	ApolloRTCCMFDData g_Data;
	int time_mode; //0 = GET, 1 = Simulation Time
	double T1; //Time of the Lambert targeted maneuver
	double T2;	//Arrival time for Lambert Targeting
	double lambertelev;
	double CDHtime;	//Time of the CDH maneuver
	double CDHtime_cor;	//Corrected time of the CDH maneuver
	int CDHtimemode; //0=Fixed, 1 = Find GETI
	double DH;			//Delta Height for the CDH maneuver
	int N;				//Number of revolutions for Lambert Targeting
	bool uni;			//displayed unit for the DV vecotr (true = m/s, false = ft/s)
	int offsetuni;		//displayed unit for the offset vector (0 = m, 1 = NM)
	int orient;			//Coordinate system used for calculations (1 = LVLH)
	VESSEL* vessel;
	VESSEL* target;
	VECTOR3 LambertdeltaV;
	int lambertopt;		//0 = spherical, 1 = non-spherical
	VECTOR3 CDHdeltaV;
	VECTOR3 offvec;
	//ApolloRTCCMFDButtons coreButtons;
	//int screen;
	double angdeg;
	int targetnumber;
	double GETbase;
	int mission; //0=manual, 7 = Apollo 7, 8 = Apollo 8, 9 = Apollo 9
	int dvdisplay;
	double apo_desnm;
	double peri_desnm;
	double incdeg;
	double SPSGET;
	VECTOR3 OrbAdjDVX;
	int iterator;
	int IterStage;
	double REFSMMATTime;
	MATRIX3 REFSMMAT;
	int REFSMMATopt; //Displayed REFSMMAT page: 0 = P30 Maneuver, 1 = P30 Retro, 2= LVLH, 3= Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC, 7 = LOI-2
	int REFSMMAToct[20];
	int REFSMMATcur; //Currently saved REFSMMAT: 0 = P30 Maneuver, 1 = P30 Retro, 2= LVLH, 3= Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC, 7 = LOI-2
	int REFSMMATupl; //0 = Desired REFSMMAT, 1 = REFSMMAT
	double LSLat, LSLng;
	OBJHANDLE gravref;
	double P30TIG;
	VECTOR3 dV_LVLH;
	//VECTOR3 dV_BRC;
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
	int entrycalcmode; //0=LEO mode with angle and longitude, 1=Entry Prediction, 2=P37 Block Data
	int entrycalcstate;
	double entryrange;
	bool SVSlot;
	VECTOR3 BRCSPos, BRCSVel;
	double BRCSGET;
	double Mantrunnion, Manshaft;
	int Manstaroct;
	double Entrytrunnion, Entryshaft;
	int Entrystaroct;
	VECTOR3 IMUangles, GDCangles;
	int GDCset;
	bool HeadsUp;
	double ManPADPeri, ManPADApo, ManPADWeight, ManPADBurnTime,ManPADDVC, ManPADPTrim, ManPADYTrim, ManPADLMWeight;
	VECTOR3 TPIPAD_dV_LOS, TPIPAD_BT;
	double TPIPAD_dH, TPIPAD_R, TPIPAD_Rdot, TPIPAD_ELmin5, TPIPAD_AZ, TPIPAD_ddH;
	double EntryPADRTGO, EntryPADVIO, EntryPADRET05Earth, EntryPADRET05Lunar, EntryPADdVTO;
	int entrypadopt; //0 = Earth Entry Update, 1 = Lunar Entry
	double EntryPADGMax, EntryPADDO;
	bool EntryPADLift;
	double EntryPADHorChkGET, EntryPADHorChkPit;
	bool REFSMMATdirect;
	int manpadopt; //0 = Maneuver PAD, 1 = TPI PAD, 2 = TLI PAD
	double EntryPADV400k, EntryPADgamma400k, EntryPADRRT;
	double EntryPADLat, EntryPADLng;
	double EntryRET05, EntryRTGO, EntryVIO;
	VECTOR3 EIangles;
	double TimeTag;
	bool EntryPADdirect;
	int ManPADSPS; //0=SPS, 1=RCS +X, 2=RCS -X
	int ManPADVeh; //0 = CSM; 1 = CSM/LM
	OBJHANDLE maneuverplanet;
	double sxtstardtime;
	double P37GET400K;
	double LOSGET, AOSGET, SSGET, SRGET, PMGET, GSAOSGET, GSLOSGET;
	int mappage, mapgs;
	bool inhibUplLOS;
	apves vesseltype;
	VESSEL* svtarget;
	int svtargetnumber;
	bool svtimemode; //0 = Now, 1 = GET
	bool lambertmultiaxis; //0 = x-axis only, 1 = multi-axis maneuver
	bool entrylongmanual; //0 = landing zone, 1 = manual longitude input
	int landingzone; //0 = Mid Pacific, 1 = East Pacific, 2 = Atlantic Ocean, 3 = Indian Ocean, 4 = West Pacific
	int entryprecision; //0 = conic, 1 = precision, 2 = PeA=-30 solution
	double EntryPADPB_RTGO, EntryPADPB_R400K, EntryPADPB_Ret05, EntryPADPB_VIO;
	int LOImaneuver; //0 = Last MCC, 1 = LOI-1 (w/ MCC), 2 = LOI-1 (w/o MCC), 3 = LOI-2
	double LOIGET, LOIPeriGET, LOILat, LOILng;
	double LOIapo, LOIperi, LOIinc;
	VECTOR3 TLCC_dV_LVLH, LOI_dV_LVLH;
	double TLCC_TIG, LOI_TIG;
	TLIPAD tlipad;
private:
	//VECTOR3 RA2, VA2, RP2, VP2;
};




#endif // !__ARCORE_H