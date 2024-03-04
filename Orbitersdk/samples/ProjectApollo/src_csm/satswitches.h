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
class SECS;
class EDA;

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

class XLunarSwitch : public SaturnToggleSwitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class SaturnH2PressureMeter : public CurvedMeter {
public:
	void Init(int i, SURFHANDLE surf, SwitchRow &row, Saturn *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	int Index;
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
};

class SaturnO2PressureMeter : public CurvedMeter {
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

class SaturnCryoQuantityMeter : public CurvedMeter {
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

class RCSQuantityMeter : public CurvedMeter {
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

class RCSFuelPressMeter : public CurvedMeter {
public:
	RCSFuelPressMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	PropellantRotationalSwitch *source;
	SURFHANDLE NeedleSurface;
};

class RCSHeliumPressMeter : public CurvedMeter {
public:
	RCSHeliumPressMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	PropellantRotationalSwitch *source;
	SURFHANDLE NeedleSurface;
};

class RCSTempMeter : public CurvedMeter {
public:
	RCSTempMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	PropellantRotationalSwitch *source;
	SURFHANDLE NeedleSurface;
};

class SaturnFuelCellMeter : public CurvedMeter {
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

class SaturnCabinMeter : public CurvedMeter {
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
	virtual void OnPostStep(double SimT, double DeltaT, double MJD);
};

class SaturnCabinPressMeter : public SaturnCabinMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	virtual void OnPostStep(double SimT, double DeltaT, double MJD);
};

class SaturnPartPressCO2Meter : public SaturnCabinMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnRoundMeter : public RoundMeter {
public:
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, Saturn *s);

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
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch);
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
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	RotationalSwitch *ECSIndicatorsSwitch;
};

class SaturnGlyEvapSteamPressMeter: public SaturnRoundMeter {
public:
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	RotationalSwitch *ECSIndicatorsSwitch;
};

class SaturnGlycolDischPressMeter: public SaturnRoundMeter {
public:
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	RotationalSwitch *ECSIndicatorsSwitch;
};

class SaturnAccumQuantityMeter: public SaturnRoundMeter {
public:
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	RotationalSwitch *ECSIndicatorsSwitch;
};

class SaturnH2oQuantityMeter: public SaturnRoundMeter {
public:
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, Saturn *s, ToggleSwitch *h2oqtyindswitch, PowerSource *src);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	ToggleSwitch *H2oQtyIndSwitch;
};

class SaturnAccelGMeter : public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	double AdjustForPower(double val) { return val; };
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

class SaturnHighGainAntennaPitchMeter: public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnHighGainAntennaStrengthMeter: public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnHighGainAntennaYawMeter: public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
};

class SaturnLMDPGauge: public SaturnRoundMeter {
public:
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void DrawNeedle (SURFHANDLE surf, int x, int y, double rad, double angle);
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
	SaturnDCAmpMeter(double minVal, double maxVal, double vMin = 210.0, double vMax = (-30.0));

	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, Saturn *s, PowerStateRotationalSwitch *dcindicatorswitch);

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

class SaturnSystemTestMeter : public DCVoltMeter
{
public:
	SaturnSystemTestMeter(double minVal, double maxVal, double vMin = 202.5, double vMax = (-22.5));
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, Saturn* s, RotationalSwitch *leftsystemtestrotaryswitch, RotationalSwitch *rightsystemtestrotaryswitch);
	double QueryValue();
protected:
	Saturn *Sat;
	RotationalSwitch *LeftSystemTestRotarySwitch;
	RotationalSwitch *RightSystemTestRotarySwitch;
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
	void InitVC(SURFHANDLE blackFontSurf, SURFHANDLE whiteFontSurf);
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void DrawSwitchVC(int id, int event, SURFHANDLE drawSurface);

protected:
	// Power is handled in SPSPropellantSource
	// These are motor driven counters, no power 
	// means no movement and NOT showing 0
	virtual double AdjustForPower(double val) { return val; };

	SURFHANDLE BlackFontSurface;
	SURFHANDLE WhiteFontSurface;
	SURFHANDLE BlackFontSurfacevc;
	SURFHANDLE WhiteFontSurfacevc;
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

class SaturnSPSPropellantPressMeter : public CurvedMeter {
public:
	void Init(SURFHANDLE surf, SwitchRow &row, 	Saturn *s, bool fuel, e_object *p);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
	bool Fuel;
};

class SaturnSPSTempMeter : public CurvedMeter {
public:
	void Init(SURFHANDLE surf, SwitchRow &row, Saturn *s, e_object *p);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
};

class SaturnSPSHeliumNitrogenPressMeter : public CurvedMeter {
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
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, Saturn *s, ToggleSwitch *lvspspcindicatorswitch, SURFHANDLE frameSurface);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	ToggleSwitch *LVSPSPcIndicatorSwitch;
	SURFHANDLE FrameSurface;
};

class SaturnGPFPIMeter : public CurvedMeter {
public:
	SaturnGPFPIMeter() { DCSource = 0; ACSource = 0; system = 0; }
	void Init(SURFHANDLE surf, SwitchRow &row, Saturn *s, int sys, int xoffset);
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);
	void WireTo(e_object *dc, e_object *ac) { DCSource = dc; ACSource = ac; };
	virtual double AdjustForPower(double val);

protected:
	SURFHANDLE NeedleSurface;
	Saturn *Sat;
	e_object *DCSource, *ACSource;
	int xOffset;
	int system;
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
	FDAIPowerRotationalSwitch() { FDAI1 = FDAI2 = NULL; ACSource1 = ACSource2 = DCSource1 = DCSource2 = NULL; GPFPIPitch1 = GPFPIPitch2 = GPFPIYaw1 = GPFPIYaw2 = NULL; eda = NULL; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, FDAI *F1, FDAI *F2, 
		      e_object *dc1, e_object *dc2, e_object *ac1, e_object *ac2, 
			  SaturnGPFPIMeter *gpfpiPitch1, SaturnGPFPIMeter *gpfpiPitch2, SaturnGPFPIMeter *gpfpiYaw1, SaturnGPFPIMeter *gpfpiYaw2, EDA *ed);

	virtual bool SwitchTo(int newValue);
	void LoadState(char *line);

protected:
	void CheckFDAIPowerState();

	FDAI *FDAI1, *FDAI2;
	SaturnGPFPIMeter *GPFPIPitch1, *GPFPIPitch2, *GPFPIYaw1, *GPFPIYaw2;
	EDA *eda;
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
	bool IsCounterClockwise() { return GetState() == 0; }

protected:
	Saturn *sat;
};

class SaturnEMSDvDisplay : public MeterSwitch {
public:
	void Init(SURFHANDLE digits, SwitchRow &row, Saturn *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void DoDrawSwitchVC(SURFHANDLE surf, double v, SURFHANDLE drawSurface);

	int GetState();
	void SetState(int value);

protected:
	virtual double AdjustForPower(double val) { return val; };

	SURFHANDLE Digits;
	Saturn *Sat;
};


// Dummy switches/displays for checklist controller

class SaturnEventTimer : public MeterSwitch {
public:
	void Init(SwitchRow &row, Saturn *s);
	double QueryValue() { return 0; }
	void DoDrawSwitch(double v, SURFHANDLE drawSurface) {};

	int GetState();
	void SetState(int value);

protected:
	Saturn *Sat;
};

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

class SaturnEMSDvSetSwitch {

public:
	SaturnEMSDvSetSwitch(Sound &clicksound);
	virtual ~SaturnEMSDvSetSwitch();
	void Init(Saturn *s) { sat = s; };
	int GetPosition() { return position; };
	bool CheckMouseClick(int event, int mx, int my);
	bool CheckMouseClickVC(int event, VECTOR3 &p);

	void DefineVCAnimations(UINT vc_idx);
	void SetReference(const VECTOR3& ref);
	void DefineMeshGroup(UINT _grp);
	void DrawSwitchVC(int id, int event, SURFHANDLE surf);

protected:
	int position;

	const VECTOR3& GetReference() const;

	VECTOR3 reference;
	VECTOR3 dir;

	UINT anim_emsdvsetswitch;
	UINT grp;

	MGROUP_ROTATE *dvswitchrot;

	Saturn *sat;
	Sound &ClickSound;
};

class SaturnCabinPressureReliefLever : public ThumbwheelSwitch
{
public:
	bool CheckMouseClickVC(int event, VECTOR3 &p);
};

class SaturnGuardedCabinPressureReliefLever: public SaturnCabinPressureReliefLever {

public:
	SaturnGuardedCabinPressureReliefLever() { guardState = 0; guardAnim = -1; };
	virtual ~SaturnGuardedCabinPressureReliefLever() {};

	void InitGuard(SURFHANDLE surf, SoundLib *soundlib);
	void DrawSwitch(SURFHANDLE drawSurface);
	void DrawSwitchVC(int id, int event, SURFHANDLE surf);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	virtual bool SwitchTo(int newState);
	void Guard();
	void SetState(int value);
	void DefineVCAnimations(UINT vc_idx);

protected:
	int guardState;
	UINT guardAnim;

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
	virtual bool SwitchTo(int newState, bool dontspring = false);
	void LoadState(char *line);
protected:
	Saturn *sat;
};

class DockingTargetSwitch : public SaturnThreePosSwitch
{
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class SaturnLiftoffNoAutoAbortSwitch :public GuardedPushSwitch
{
public:
	SaturnLiftoffNoAutoAbortSwitch();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, SECS *s,
		int xoffset = 0, int yoffset = 0, int lxoffset = 0, int lyoffset = 0);
	void DoDrawSwitch(SURFHANDLE drawSurface);
	void RepaintSwitchVC(SURFHANDLE drawSurface, SURFHANDLE switchsurfacevc, int xTexMul = 1);
protected:
	SECS * secs;
};

class PanelSwitchScenarioHandler;

class SaturnPanel181 : public BasicPanel
{
public:

	void Register(PanelSwitchScenarioHandler *PSH);

	SwitchRow SMSector1SwitchesRow;
	ToggleSwitch SMSector1Cryo3ACPowerSwitch;
	ToggleSwitch SMSector1SMACPowerSwitch;
	CircuitBrakerSwitch SMSector1AC2ASystemBraker;
	CircuitBrakerSwitch SMSector1AC2BSystemBraker;
	CircuitBrakerSwitch SMSector1AC2CSystemBraker;
	GuardedToggleSwitch SMSector1DoorJettisonSwitch;
	ThreePosSwitch SMSector1LogicPower1Switch;
	ThreePosSwitch SMSector1LogicPower2Switch;
	CircuitBrakerSwitch SMSector1LogicPowerMNABraker;
	CircuitBrakerSwitch SMSector1LogicPowerMNBBraker;
};

class SaturnPanel277 : public BasicPanel
{
public:

	void Register(PanelSwitchScenarioHandler *PSH);

	SwitchRow Panel277SwitchesRow;
	ToggleSwitch SPSPressIndFuelSwitch;
	ToggleSwitch SPSPressIndOxidSwitch;
	CircuitBrakerSwitch CSMLMFinalSepABatABraker;
	CircuitBrakerSwitch CSMLMFinalSepBBatBBraker;
};

class SaturnPanel278J : public BasicPanel
{
public:

	void Register(PanelSwitchScenarioHandler *PSH);

	SwitchRow Panel278JSwitchesRow;
	ThreePosSwitch ExperimentCovers1Switch;
	ThreePosSwitch ExperimentCovers2Switch;
	GuardedThreePosSwitch SMPowerSourceSwitch;
	ThreePosSwitch O2Tank3IsolSwitch;
	IndicatorSwitch ExperimentCovers1Indicator;
	IndicatorSwitch ExperimentCovers2Indicator;
	IndicatorSwitch O2Tank3IsolIndicator;
	CircuitBrakerSwitch ExperimentCoversDeployBraker;
};

class LeftCOASPowerSwitch : public SaturnToggleSwitch
{
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class SaturnAltimeter
{
public:
	SaturnAltimeter() { Sat = NULL; animNeedle = -1; };
	void Init(SURFHANDLE surf1, SURFHANDLE surf2, Saturn *s);
	void RedrawPanel_Alt(SURFHANDLE surf);
	void RedrawPanel_Alt2(SURFHANDLE surf);
	void DrawSwitchVC(int id, int event, SURFHANDLE surf);
	void DrawNeedle(oapi::Sketchpad *skp, int x, int y, double rad, double angle, oapi::Pen *pen0, oapi::Pen *pen1);
	void DefineVCAnimations(UINT vc_idx);

protected:
	Saturn *Sat;
	SURFHANDLE surface1;
	SURFHANDLE surface2;

	UINT animNeedle;
};