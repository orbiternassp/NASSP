// IMFD_IPC_com.h
//
// Definition of IPC shared memory blocks for
// communication between IMFD and vessels
// Version: 1.02

// ****************************************************************
// IMFD version 5.01 and over, include now an IPC Memory blocks
// for external communication purposes.
//
// This memory block is called IMFD_COM_IPC_100 and allow useful
// datas exchanges between IMFD and any vessels.
//
// To know how to connect and use to this IPC memory block, please
// refer to the file "IMFD_IPC_sample.cpp".
// 
// WARNING: Your code MUST be compiled with "Struct member aligment"
// generation code parameter set to default "8 Bytes", to insure a
// correct access to the memory block datas.
//
// ****************************************************************


/**** CHANGE LOG **************************************************

06-Oct-2007: * _dV_LVLH operation changed in P30 compatible mode 
			 * IPC_LVLH flag obsollete	
			 * Z - Axis inverted from LVLH code
			 * p30mode boolean added

15-Dec-2007: * Offset setup added
			 * ApolloP30 auto-configuration option added

*******************************************************************/




// Name of COM IPC memory block
#define IMFD_COM_AREA_NAME "IMFD_COM_IPC_100"

#define IPC_BURN 1
#define IPC_LVLH 2	// obsollete


// Global To LV conversion
//
//VECTOR3 LVLH(VECTOR3 _in, VECTOR3 _pos, VECTOR3 _vel)
//{
//	VECTOR3 _out;
//	VECTOR3 _prod = crossp(_pos,_vel);
//	VECTOR3 _horz = crossp(_prod,_pos);
//	
//	_out.x = dotp(unit(_horz), _in);
//	_out.z = -dotp(unit(_pos), _in);
//	_out.y = dotp(unit(_prod), _in);
//
//	return _out;
//}



typedef struct BurnData {
	
	int		ID;				// ID of the program that created the data
							// Hexadecimal 0xPPMM, where PP is a program and MM is a mode identifier

	double	DataMJD;		// Time when this data is created
							// Non-Zero DataMJD indicates valid data  (all except LAP)

	bool	impulsive;		// TRUE indicates impulsive maneuver in witch:

							// _RIgn = Radius at Ignition and Vessel current radius after ignition 
							// _VIgn = Velocity at Ignition and Vessel current velocity after ignition

							// _RCut = _RIgn,  
							// _VCut = _VIgn + _Thr,  _Thr = _dV = remaining delta velovity
	
							// FALSE indicates non-impulsive maneuver

	bool	p30mode;		// true when IMFD operates in P30 compatibility mode
			
	OBJHANDLE Reference;	// Reference Object

	VECTOR3 _RIgn, _VIgn;	// Ignition State Vectors in Global Frame
	VECTOR3 _RCut, _VCut;	// Cut-Off State Vectors (Resulting transfer orbit is defined by these parameters) 

							// Powered flight steering errors makes _dV parameters less accurate. 

	// ----- Thrust Vector ----------------------------------------------------------------------------------

							// ATTENTION: Not unit vector anymore.

	VECTOR3 _Thr;			// Thrust Vector in Velocity Frame x=prograde, y=orbit anti-normal, z=outbound
							// PlaneChange Thrust Angle = atan(z/x)
							// Outbound Thrust Angle    = atan(y/x)
	
							// Thrust Vector (delta velocity) in Global Frame if (p30mode=true)
							// Delta-Velocity Vector in Global Frame if (impulsive==true) 
							

	// ------ dV_LVLH ---------------------------------------------------------------------------------------						


	VECTOR3 _dV_LVLH;		// This is a P30 input velocity in LVLH system (metric)
							// This information is available in p30mode 
							// This information is available in impulsive maneuver mode.
							// This inforamtion is _NULL if (impulsive==false) and (p30mode==false)

	// ---------------------------------------------------------------------------------------------

	double	 IgnMJD;		// Ignition MJD
	double	 BT;			// Burn Time    ( approximated in impulsive maneuver )		

	double	 IgnMass;		// Ignition Mass
	double   CutMass;		// Cut-Off Mass ( approximated in impulsive maneuver )
	

	//--------- Engine --------
	double	 MaxThrust;		// MaxThrust in newtons
	double	 ISP;
	
	THGROUP_TYPE EngineType;


	//---- Lambert Aim Point ----
	VECTOR3 _LAP;			// Lambert Aim Point for precise fine tuning
	double   LAP_MJD;		// MJD of Lambert Aim Point  (Non-Zero LAP_MJD indicates valid LAP information)

							// LAP information of PlanetApproach ID=0x0101, BaseApproach Orbit Insert ID=0x0703,
							// BaseApproach ReEntry ID=0x0701, Target Intercept (realtime) ID=0x0202 is never tested or used by IMFD.							
} IMFD_BURN_DATA;




// IPC COM data structure
typedef struct IMFDComSet {

	// IMFD DOWNLINK
	// -------------

	OBJHANDLE RequestToVessel; // Vessel handle to which request is adressed.
	bool	ReqToDone;		// True when vessel has responded.
	bool	ReqVar;			// True for variable data request.
	bool	ReqConst;		// True for constant data request.
							// REMARK: vessel must support a single request for both data.							
	// VARIABLE DATA(S)
	double	DataTimeStamp;	// Orbiter simt when the variable data was written into IPC.
	double	GET;			// Ground Elapsed Time in sec, equal to -1 before launch.
	char	VarReserved[64];

	// CONSTANT DATA(S)
	char	LandingBase[64]; // Name of the landing base used for the mission.	
							 //* This is equal to the secundary target in configuration program
							 //* Currently used only by Map program to compute base alignment among with MJDLDG.

	char    TargetCelbody[64]; // Target Celbody for the Target Intercept program.
							   
	char	Reserved2[128];
							 
	
	double	AZIMUTCOR;		// Launch azimut correction (see AMSO doc), NULL if not available.
	double	MJDTLI;			// MJD of TLI burn, NULL if not available.
							//* This is default TEj input for Target Intercept program.

	double	MJDLOI;			// MJD of first LOI burn, NULL if not available.
							//* This is default TIn input for target intercept.
	
	double	MJDPEC;			// MJD of free return pericynthion, NULL if not available.
	double	ALTPEC;			// Altitude of free return pericynthion, NULL if not available.
							//* These are not currently used by IMFD

	double	MJDLDG;			// MJD of lunar landing, NULL if not available.
							//* This is only used by Map program among with LandingBase input

	double	MJDSPL;			// MJD of normal return entry interface, NULL if not available.
	double	SPL_lon;		// Normal return splachdown long in decimal deg, if MJDSPL valid.
	double	SPL_lat;		// Normal return splachdown lat in decimal deg, if MJDSPL valid.
							//* Default settings for BaseApproach program and these are only used in Earth re-entry

	char    ConstReserved[64];

	// IMFD UPLINK
	// -----------
	OBJHANDLE RequestFromVessel; // Vessel handle from which request come.
	bool	ReqFromDone;	// True when IMFD has responded.

	// BURN DATAS
	int     BurnFlags;		// Flags for data requests. NULL means no burn data required.
							// Flags = IPC_BURN|IPC_LVLH fills everything sofar.
							// There is a delay of one second before data is available.
	IMFD_BURN_DATA Burn;	// see structure definition before.



	// IMFD DOWNLINK
	// -------------

	bool	ApolloP30;		// When true will force LambertAP=ApolloP30 and Off-Axis guidance when available.
							// Auto-Configuration is done during:
							// -activation of Course program
							// -activation of BaseApproach
						

	double	OffsetLon;		// In Radians
	double  OffsetLat;      // In Radians
	double  OffsetRad;		// Meters, (also enables offset in Vel.Frame when non-zero)

} IMFDCOMSET;
