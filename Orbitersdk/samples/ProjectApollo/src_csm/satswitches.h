/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn-specific switches

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
  *	Revision 1.6  2010/07/16 17:14:42  tschachim
  *	Changes for Orbiter 2010 and bugfixes
  *	
  *	Revision 1.5  2010/02/05 17:31:46  tschachim
  *	Added ORDEAL.
  *	
  *	Revision 1.4  2009/12/17 17:47:18  tschachim
  *	New default checklist for ChecklistMFD together with a lot of related bugfixes and small enhancements.
  *	
  *	Revision 1.3  2009/08/16 03:12:38  dseagrav
  *	More LM EPS work. CSM to LM power transfer implemented. Optics bugs cleared up.
  *	
  *	Revision 1.2  2009/08/10 14:38:03  tschachim
  *	ECS enhancements
  *	
  *	Revision 1.1  2009/02/18 23:20:56  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.34  2008/12/07 18:35:21  movieman523
  *	Very basics of DSE telemetry recording: the play/record switch works but nothing else does!
  *	
  *	Also don't try to create a panel if we can't load the bitmap.
  *	
  *	Revision 1.33  2008/04/11 12:19:11  tschachim
  *	New SM and CM RCS.
  *	Improved abort handling.
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.32  2008/01/25 04:39:42  lassombra
  *	All switches now handle change of state through SwitchTo function which is vitual
  *	 and is called by existing mouse and connector handling methods.
  *	
  *	Support for delayed spring switches and other ChecklistController functionality following soon.
  *	
  *	Revision 1.31  2008/01/14 01:17:07  movieman523
  *	Numerous changes to move payload creation from the CSM to SIVB.
  *	
  *	Revision 1.30  2007/11/29 22:08:27  movieman523
  *	Moved electric meters to generic classes in toggleswitch.cpp rather than Saturn-specific.
  *	
  *	Revision 1.29  2007/11/29 21:53:20  movieman523
  *	Generising the Volt meters.
  *	
  *	Revision 1.28  2007/11/29 21:28:44  movieman523
  *	Electrical meters now use a common base class which handles the rendering.
  *	
  *	Revision 1.27  2007/11/29 04:56:09  movieman523
  *	Made the System Test meter work (though currently it's connected to the rotary switch, which isn't connected to anything, so just displays 0V).
  *	
  *	Revision 1.26  2007/10/18 00:23:20  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.25  2007/08/13 16:06:16  tschachim
  *	Moved bitmaps to subdirectory.
  *	New VAGC mission time pad load handling.
  *	New telescope and sextant panels.
  *	Fixed CSM/LV separation speed.
  *	
  *	Revision 1.24  2007/07/17 14:33:08  tschachim
  *	Added entry and post landing stuff.
  *	
  *	Revision 1.23  2007/04/25 18:48:10  tschachim
  *	EMS dV functions.
  *	
  *	Revision 1.22  2007/01/22 14:54:09  tschachim
  *	Moved FDAIPowerRotationalSwitch from toggleswitch, added SPS TVC displays & controls.
  *	
  *	Revision 1.21  2007/01/14 13:02:42  dseagrav
  *	CM AC bus feed reworked. Inverter efficiency now varies, AC busses are 3-phase all the way to the inverter, inverter switching logic implemented to match the CM motor-switch lockouts. Original AC bus feeds deleted. Inverter overload detection enabled and correct.
  *	
  *	Revision 1.20  2006/12/19 15:56:05  tschachim
  *	ECS test stuff, bugfixes.
  *	
  *	Revision 1.19  2006/11/13 14:47:30  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.18  2006/06/17 18:13:13  tschachim
  *	Moved BMAGPowerRotationalSwitch.
  *	
  *	Revision 1.17  2006/06/10 14:36:44  movieman523
  *	Numerous changes. Lots of bug-fixes, new LES jettison code, lighting for guarded push switches and a partial rewrite of the Saturn 1b mesh code.
  *	
  *	Revision 1.16  2006/05/30 22:34:33  movieman523
  *	Various changes. Panel switches now need power, APO and PER correctly placed in scenario fle, disabled some warnings, moved 'window' sound message to the correct place, added heat measurement to SM DLL for re-entry.
  *	
  *	Revision 1.15  2006/05/30 14:40:21  tschachim
  *	Fixed fuel cell - dc bus connectivity, added battery charger
  *	
  *	Revision 1.14  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.13  2006/04/25 13:52:06  tschachim
  *	Removed GetXXXSwitchState.
  *	
  *	Revision 1.12  2006/04/17 18:14:27  movieman523
  *	Added flashing borders to all switches (I think).
  *	
  *	Revision 1.11  2006/03/27 19:22:44  quetalsi
  *	Bugfix RCS PRPLNT switches and wired to brakers.
  *	
  *	Revision 1.10  2006/02/22 18:50:51  tschachim
  *	Bugfixes for Apollo 4-6.
  *	
  *	Revision 1.9  2006/02/02 18:52:35  tschachim
  *	Improved Accel G meter.
  *	
  *	Revision 1.8  2006/02/01 18:18:22  tschachim
  *	Added SaturnValveSwitch::SwitchTo function.
  *	
  *	Revision 1.7  2006/01/11 02:59:43  movieman523
  *	Valve talkbacks now check the valve state directlry. This means they barberpole on SM sep and can't then be changed.
  *	
  *	Revision 1.6  2006/01/11 02:16:25  movieman523
  *	Added RCS propellant quantity gauge.
  *	
  *	Revision 1.5  2005/10/31 10:38:24  tschachim
  *	Offset for SaturnToggleSwitch, SPSSwitch is now 2-pos.
  *	
  *	Revision 1.4  2005/09/30 11:22:40  tschachim
  *	Added ECS meters.
  *	
  *	Revision 1.3  2005/08/23 22:18:47  movieman523
  *	SPS switch now works.
  *	
  *	Revision 1.2  2005/08/23 20:13:12  movieman523
  *	Added RCS talkbacks and changed AGC to use octal addresses for EMEM.
  *	
  *	Revision 1.1  2005/08/16 20:55:23  movieman523
  *	Added first saturn-specific switch for Xlunar Inject.
  *	
  **************************************************************************/

class Saturn;
class DCBusController;
class BMAG;
class RCSValve;
class SMRCSHeliumValve;
class RCSPropellantValve;
class SMRCSPropellantSource;
class CMRCSPropellantSource;
class DSE;

class SaturnToggleSwitch : public ToggleSwitch {
public:
	SaturnToggleSwitch() { sat = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s, int xoffset = 0, int yoffset = 0);

protected:
	Saturn *sat;
};

class SaturnThreePosSwitch : public ThreePosSwitch {
public:
	SaturnThreePosSwitch() { sat = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s);

protected:
	Saturn *sat;
};

class SaturnRCSValveTalkback : public IndicatorSwitch {
public:
	SaturnRCSValveTalkback();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, RCSValve *v, bool failopen);
	int GetState();

protected:
	RCSValve *valve;
};

class SaturnGuardedPushSwitch : public GuardedPushSwitch
{
public:
	SaturnGuardedPushSwitch() { sat = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *v, int xoffset = 0, int yoffset = 0, int lxoffset = 0, int lyoffset = 0);

protected:
	Saturn *sat;
};

class LESMotorFireSwitch : public SaturnGuardedPushSwitch
{
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class XLunarSwitch : public SaturnToggleSwitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class SaturnH2PressureMeter : public MeterSwitch {
public:
	void Init(int i, SURFHANDLE surf, SwitchRow &row, Saturn *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	int Index;
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
};

class SaturnO2PressureMeter : public MeterSwitch {
public:
	void Init(int i, SURFHANDLE surf, SwitchRow &row, Saturn *s, ToggleSwitch *o2PressIndSwitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	int Index;
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
	ToggleSwitch *O2PressIndSwitch;	

	void DoDrawSwitch(SURFHANDLE surf, SURFHANDLE needle, double value, int xOffset, int xNeedle);
};

class SaturnCryoQuantityMeter : public MeterSwitch {
public:
	void Init(char *sub, int i, SURFHANDLE surf, SwitchRow &row, Saturn *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	virtual double AdjustForPower(double val) { return (Voltage() < SP_MIN_ACVOLTAGE ? 0 : val); };

	char *Substance;
	int Index;
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
};

class PropellantSource {
public:
	PropellantSource(PROPELLANT_HANDLE &h);
	void SetVessel(Saturn *v) { our_vessel = v; };
	PROPELLANT_HANDLE Handle();
	virtual double Quantity();
	virtual double Temperature() { return 0.0; };
	virtual double Pressure() { return 0.0; };

protected:
	PROPELLANT_HANDLE &source_prop;
	Saturn *our_vessel;
};

class PropellantRotationalSwitch: public RotationalSwitch {
public:
	PropellantRotationalSwitch();
	void SetCMSource(int num, CMRCSPropellantSource *s);
	void SetSMSource(int num, SMRCSPropellantSource *s);
	CMRCSPropellantSource *GetCMSource();
	SMRCSPropellantSource *GetSMSource();

protected:
	CMRCSPropellantSource *CMSources[7];
	SMRCSPropellantSource *SMSources[7];
};

class RCSQuantityMeter : public MeterSwitch {
public:
	RCSQuantityMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s, ToggleSwitch *indswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	PropellantRotationalSwitch *source;
	ToggleSwitch *SMRCSIndSwitch;
	SURFHANDLE NeedleSurface;
};

class RCSFuelPressMeter : public MeterSwitch {
public:
	RCSFuelPressMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	PropellantRotationalSwitch *source;
	SURFHANDLE NeedleSurface;
};

class RCSHeliumPressMeter : public MeterSwitch {
public:
	RCSHeliumPressMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	PropellantRotationalSwitch *source;
	SURFHANDLE NeedleSurface;
};

class RCSTempMeter : public MeterSwitch {
public:
	RCSTempMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	PropellantRotationalSwitch *source;
	SURFHANDLE NeedleSurface;
};

class SaturnFuelCellMeter : public MeterSwitch {
public:
	void Init(SURFHANDLE surf, SwitchRow &row, Saturn *s, RotationalSwitch *fuelCellIndicatorsSwitch);

protected:
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
	RotationalSwitch *FuelCellIndicatorsSwitch;
};

class SaturnFuelCellH2FlowMeter : public SaturnFuelCellMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnFuelCellO2FlowMeter : public SaturnFuelCellMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnFuelCellTempMeter : public SaturnFuelCellMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnFuelCellCondenserTempMeter : public SaturnFuelCellMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnCabinMeter : public MeterSwitch {
public:
	void Init(SURFHANDLE surf, SwitchRow &row, Saturn *s);

protected:
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
};

class SaturnSuitTempMeter : public SaturnCabinMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnCabinTempMeter : public SaturnCabinMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnSuitPressMeter : public SaturnCabinMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnCabinPressMeter : public SaturnCabinMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnPartPressCO2Meter : public SaturnCabinMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnRoundMeter : public RoundMeter {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s);

protected:
	Saturn *Sat;
};

class SaturnLeftO2FlowMeter : public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnSuitComprDeltaPMeter : public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnSuitCabinDeltaPMeter : public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

	SURFHANDLE FrameSurface;
};

class SaturnRightO2FlowMeter : public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

	SURFHANDLE FrameSurface;
};

class SaturnEcsRadTempInletMeter: public SaturnRoundMeter {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	RotationalSwitch *ECSIndicatorsSwitch;
};

class SaturnEcsRadTempPrimOutletMeter: public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnEcsRadTempSecOutletMeter: public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnGlyEvapTempOutletMeter: public SaturnRoundMeter {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	RotationalSwitch *ECSIndicatorsSwitch;
};

class SaturnGlyEvapSteamPressMeter: public SaturnRoundMeter {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	RotationalSwitch *ECSIndicatorsSwitch;
};

class SaturnGlycolDischPressMeter: public SaturnRoundMeter {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	RotationalSwitch *ECSIndicatorsSwitch;
};

class SaturnAccumQuantityMeter: public SaturnRoundMeter {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	RotationalSwitch *ECSIndicatorsSwitch;
};

class SaturnH2oQuantityMeter: public SaturnRoundMeter {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, ToggleSwitch *h2oqtyindswitch, CircuitBrakerSwitch *cba, CircuitBrakerSwitch *cbb);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	ToggleSwitch *H2oQtyIndSwitch;
	CircuitBrakerSwitch *CbA;
	CircuitBrakerSwitch *CbB;
};

class SaturnAccelGMeter : public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class DirectO2RotationalSwitch: public RotationalSwitch {
public:
	DirectO2RotationalSwitch() { Pipe = NULL; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, h_Pipe *p);
	virtual bool SwitchTo(int newValue);

protected:
	void CheckValve();

	h_Pipe *Pipe;
};

class SaturnEcsGlycolPumpsSwitch: public RotationalSwitch {
public:
	SaturnEcsGlycolPumpsSwitch(PanelSDK &p) : ACBus1(0, p), ACBus2(0, p) { GlycolPump = NULL; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Pump *p,
		      CircuitBrakerSwitch* ac1a, CircuitBrakerSwitch* ac1b, CircuitBrakerSwitch* ac1c,
			  CircuitBrakerSwitch* ac2a, CircuitBrakerSwitch* ac2b, CircuitBrakerSwitch* ac2c);
	virtual bool SwitchTo(int newValue);
	void LoadState(char *line);

protected:
	void CheckPump();

	Pump *GlycolPump;
	ThreeWayPowerMerge ACBus1;
	ThreeWayPowerMerge ACBus2;
};

class SaturnSuitCompressorSwitch: public ThreeSourceSwitch {
public:
	SaturnSuitCompressorSwitch(PanelSDK &p) : ACBus1(0, p), ACBus2(0, p) { };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row,
		      CircuitBrakerSwitch* ac1a, CircuitBrakerSwitch* ac1b, CircuitBrakerSwitch* ac1c,
			  CircuitBrakerSwitch* ac2a, CircuitBrakerSwitch* ac2b, CircuitBrakerSwitch* ac2c);

protected:
	ThreeWayPowerMerge ACBus1;
	ThreeWayPowerMerge ACBus2;
};

///
/// \brief DC current meter for Saturn panel.
///
/// \image html DCAmps.bmp "DC current meter"
///
/// This meter displays the DC current of one of the numerous DC systems in the CSM (e.g. the main buses
/// or batteries).
///
/// \ingroup PanelItems
///
class SaturnDCAmpMeter: public ElectricMeter {
public:
	///
	/// \brief Constructor.
	/// \param minVal Minimum current to display (meter may show beyond it).
	/// \param maxVal Maximum current to display (meter may show beyond it).
	/// \param vMin Angle of meter at minimum current.
	/// \param vMax Angle of meter at maximum current.
	///
	SaturnDCAmpMeter(double minVal, double maxVal, double vMin = 202.5, double vMax = (-22.5));

	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, PowerStateRotationalSwitch *dcindicatorswitch);

	///
	/// \brief Query the voltage.
	/// \return Current voltage.
	///
	double QueryValue();

protected:
	///
	/// \brief The switch we're connected to.
	///
	/// The DC Amp meter in the Saturn is connected to a switch which can show different ranges based on
	/// position. So we need to store it and get the state to determine the scale factor.
	///
	PowerStateRotationalSwitch *DCIndicatorSwitch;
};

class DCBusIndicatorSwitch: public IndicatorSwitch {
public:
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, DCBusController *d, int fc);
	int GetState();

protected:
	DCBusController *dcbus;
	int fuelcell;
};

class BMAGPowerRotationalSwitch: public RotationalSwitch {
public:
	BMAGPowerRotationalSwitch() { bmag = NULL; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, BMAG *Unit);

	virtual bool SwitchTo(int newValue);
	void LoadState(char *line);

protected:
	void CheckBMAGPowerState();

	BMAG *bmag;	
};

class SaturnSPSPercentMeter : public MeterSwitch {
public:
	void Init(SURFHANDLE blackFontSurf, SURFHANDLE whiteFontSurf, SwitchRow &row, Saturn *s);
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	// Power is handled in SPSPropellantSource
	// These are motor driven counters, no power 
	// means no movement and NOT showing 0
	virtual double AdjustForPower(double val) { return val; };

	SURFHANDLE BlackFontSurface;
	SURFHANDLE WhiteFontSurface;
	Saturn *Sat;
};

class SaturnSPSOxidPercentMeter : public SaturnSPSPercentMeter {
public:
	double QueryValue();
};

class SaturnSPSFuelPercentMeter : public SaturnSPSPercentMeter {
public:
	double QueryValue();
};

class SaturnSPSOxidUnbalMeter : public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	// Power is handled in SPSPropellantSource
	virtual double AdjustForPower(double val) { return val; };
};

class SaturnSPSPropellantPressMeter : public MeterSwitch {
public:
	void Init(SURFHANDLE surf, SwitchRow &row, 	Saturn *s, bool fuel);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
	bool Fuel;
};

class SaturnSPSTempMeter : public MeterSwitch {
public:
	void Init(SURFHANDLE surf, SwitchRow &row, Saturn *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
};

class SaturnSPSHeliumNitrogenPressMeter : public MeterSwitch {
public:
	void Init(SURFHANDLE surf, SwitchRow &row, Saturn *s, ThreePosSwitch *spspressindswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
	ThreePosSwitch *SPSPressIndSwitch;
};

class SaturnLVSPSPcMeter : public SaturnRoundMeter {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, ToggleSwitch *lvspspcindicatorswitch, SURFHANDLE frameSurface);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	ToggleSwitch *LVSPSPcIndicatorSwitch;
	SURFHANDLE FrameSurface;
};

class SaturnGPFPIMeter : public MeterSwitch {
public:
	SaturnGPFPIMeter() { DCSource = 0; ACSource = 0; }
	void Init(SURFHANDLE surf, SwitchRow &row, Saturn *s, ToggleSwitch *gpfpiindswitch, int xoffset);
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void WireTo(e_object *dc, e_object *ac) { DCSource = dc; ACSource = ac; };
	virtual double AdjustForPower(double val);

protected:
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
	ToggleSwitch *GPFPIIndicatorSwitch;
	e_object *DCSource, *ACSource;
	int xOffset;
};

class SaturnGPFPIPitchMeter : public SaturnGPFPIMeter {
public:
	double QueryValue();
};

class SaturnGPFPIYawMeter : public SaturnGPFPIMeter {
public:
	double QueryValue();
};

class FDAIPowerRotationalSwitch: public RotationalSwitch {
public:
	FDAIPowerRotationalSwitch() { FDAI1 = FDAI2 = NULL; ACSource1 = ACSource2 = DCSource1 = DCSource2 = NULL; GPFPIPitch1 = GPFPIPitch2 = GPFPIYaw1 = GPFPIYaw2 = NULL; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, FDAI *F1, FDAI *F2, 
		      e_object *dc1, e_object *dc2, e_object *ac1, e_object *ac2, 
			  SaturnGPFPIMeter *gpfpiPitch1, SaturnGPFPIMeter *gpfpiPitch2, SaturnGPFPIMeter *gpfpiYaw1, SaturnGPFPIMeter *gpfpiYaw2);

	virtual bool SwitchTo(int newValue);
	void LoadState(char *line);

protected:
	void CheckFDAIPowerState();

	FDAI *FDAI1, *FDAI2;
	SaturnGPFPIMeter *GPFPIPitch1, *GPFPIPitch2, *GPFPIYaw1, *GPFPIYaw2;
	e_object *DCSource1, *DCSource2, *ACSource1, *ACSource2;
};

///
/// A two-position switch which operates the CM AC inverter motor-switches.
/// \brief CM AC Inverter Switch
///
class CMACInverterSwitch : public ToggleSwitch {
public:
	CMACInverterSwitch() { acbus = 0; acinv = 0; sat = NULL; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row,int bus,int inv,Saturn *ship);
	virtual bool SwitchTo(int newState, bool dontspring = true);
	void LoadState(char *line);
	virtual void UpdateSourceState();

protected:
	int acbus,acinv;
	Saturn *sat;	
};

class SaturnSCControlSetter {
public:
	void SetSCControl(Saturn *sat);
};

class SaturnSCContSwitch : public SaturnToggleSwitch, public SaturnSCControlSetter {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class THCRotarySwitch : public RotationalSwitch, public SaturnSCControlSetter  {
public:
	THCRotarySwitch() { sat = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s);
	virtual bool SwitchTo(int newValue);
	bool IsClockwise() { return GetState() == 2; }
	bool IsCounterClockwise() { return GetState() == 3; }

protected:
	Saturn *sat;
};

class SaturnEMSDvDisplay : public MeterSwitch {
public:
	void Init(SURFHANDLE digits, SwitchRow &row, Saturn *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

	int GetState();
	void SetState(int value);

protected:
	virtual double AdjustForPower(double val) { return val; };

	SURFHANDLE Digits;
	Saturn *Sat;
};


// Dummy switches/displays for checklist controller

class SaturnEMSScrollDisplay : public MeterSwitch {
public:
	void Init(SwitchRow &row, Saturn *s);
	double QueryValue() { return 0; }
	void DoDrawSwitch(double v, SURFHANDLE drawSurface) {};

	int GetState();
	void SetState(int value);

protected:
	Saturn *Sat;
};

class SaturnPanel382Cover : public MeterSwitch {
public:
	void Init(SwitchRow &row, Saturn *s);
	double QueryValue() { return 0; }
	void DoDrawSwitch(double v, SURFHANDLE drawSurface) {};

	int GetState();
	void SetState(int value);

protected:
	Saturn *Sat;
};

class SaturnPanel600 : public MeterSwitch {
public:
	void Init(SwitchRow &row, Saturn *s);
	double QueryValue() { return 0; }
	void DoDrawSwitch(double v, SURFHANDLE drawSurface) {};

	int GetState();
	void SetState(int value);

protected:
	Saturn *Sat;
};

class SaturnPanelOrdeal : public MeterSwitch {
public:
	void Init(SwitchRow &row, Saturn *s);
	double QueryValue() { return 0; }
	void DoDrawSwitch(double v, SURFHANDLE drawSurface) {};

	int GetState();
	void SetState(int value);

protected:
	Saturn *Sat;
};

class SaturnASCPSwitch : public MeterSwitch {
public:
	void Init(SwitchRow &row, Saturn *s, int axis);
	double QueryValue() { return 0; }
	void DoDrawSwitch(double v, SURFHANDLE drawSurface) {};

	int GetState();
	void SetState(int value);

protected:
	Saturn *Sat;
	int Axis;
};

class SaturnAbortSwitch : public MeterSwitch {
public:
	void Init(SwitchRow &row, Saturn *s);
	double QueryValue() { return 0; }
	void DoDrawSwitch(double v, SURFHANDLE drawSurface) {};

	int GetState();
	void SetState(int value);

protected:
	Saturn *Sat;
};

class SaturnEMSDvSetSwitch {

public:
	SaturnEMSDvSetSwitch(Sound &clicksound);
	void Init(Saturn *s) { sat = s; };
	int GetPosition() { return position; };
	bool CheckMouseClick(int event, int mx, int my);

protected:
	int position;
	Saturn *sat;
	Sound &ClickSound;
};

class SaturnCabinPressureReliefLever: public ThumbwheelSwitch {

public:
	SaturnCabinPressureReliefLever() { guardState = 0; };
	virtual ~SaturnCabinPressureReliefLever() { guardClick.done(); };

	void InitGuard(SURFHANDLE surf, SoundLib *soundlib);
	void DrawSwitch(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	virtual bool SwitchTo(int newState);

protected:
	int guardState;

	Sound guardClick;
	SURFHANDLE guardSurface;
};

class OpticsHandcontrollerSwitch: public HandcontrollerSwitch {

public:
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s);
	bool CheckMouseClick(int event, int mx, int my);

protected:
	Saturn *sat;
};

class MinImpulseHandcontrollerSwitch: public HandcontrollerSwitch {

public:
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s);
	bool CheckMouseClick(int event, int mx, int my);

protected:
	Saturn *sat;
};

class SuitTestSwitch: public RotationalSwitch {

public:
	virtual void DrawSwitch(SURFHANDLE drawSurface);
	virtual void DrawFlash(SURFHANDLE DrawSurface);
	virtual bool CheckMouseClick(int event, int mx, int my);
};

class DSEIndicatorSwitch : public IndicatorSwitch
{
public:
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, DSE *d, bool failopen = false);
	int GetState();

protected:
	DSE *dse;
};

class SaturnOxygenRepressPressMeter : public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	SURFHANDLE FrameSurface;
};

class CSMLMPowerSwitch : public SaturnThreePosSwitch 
{
public:
	CSMLMPowerSwitch() { sat = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s);
	bool CSMLMPowerSwitch::CheckMouseClick(int event, int mx, int my);
	bool CSMLMPowerSwitch::SwitchTo(int newState);
protected:
	Saturn *sat;
};

class OrdealRotationalSwitch: public RotationalSwitch {

public:
	OrdealRotationalSwitch() { value = 100; lastX = 0; mouseDown = false; };
	virtual void DrawSwitch(SURFHANDLE drawSurface);
	virtual bool CheckMouseClick(int event, int mx, int my);
	virtual void SaveState(FILEHANDLE scn);
	virtual void LoadState(char *line);
	int GetValue() { return value; }

protected:
	int value;
	int lastX;
	bool mouseDown;
};
