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

class SaturnValveSwitch: public SaturnThreePosSwitch {
public:
	SaturnValveSwitch() { Valve = 0; Indicator = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s, int valve, IndicatorSwitch *ind);
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

protected:
	void CheckValve(int s);

	int Valve;
	IndicatorSwitch *Indicator;
};

class SaturnPropValveSwitch: public SaturnThreePosSwitch {
public:
	SaturnPropValveSwitch() { Valve1 = 0; Valve2 = 0; Valve3 = 0; Valve4 = 0; Indicator1 = 0; Indicator2 = 0;};
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s, int valve1, int valve2, int valve3,
		int valve4,	IndicatorSwitch *ind1, IndicatorSwitch *ind2);
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

protected:
	void CheckValve(int s);

	int Valve1, Valve2, Valve3, Valve4;
	IndicatorSwitch *Indicator1, *Indicator2;
};

class SaturnValveTalkback : public IndicatorSwitch {
public:
	SaturnValveTalkback();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int vlv, Saturn *v);
	int GetState();

protected:
	int Valve;
	Saturn *our_vessel;
};

class SaturnPropValveTalkback : public IndicatorSwitch {
public:
	SaturnPropValveTalkback();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int vlv1, int vlv2, Saturn *v);
	int GetState();

protected:
	int Valve1, Valve2;
	Saturn *our_vessel;
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
	bool CheckMouseClick(int event, int mx, int my);
};

class XLunarSwitch : public SaturnToggleSwitch {
public:
	bool CheckMouseClick(int event, int mx, int my);
};

class SaturnSPSSwitch : public SaturnToggleSwitch {
public:
	bool CheckMouseClick(int event, int mx, int my);
	void SetState(bool s);
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

class CMRCSPropellant : public PropellantSource {
public:
	CMRCSPropellant(PROPELLANT_HANDLE &h);
	double Quantity();
};

class SMRCSPropellant : public PropellantSource {
public:
	SMRCSPropellant(PROPELLANT_HANDLE &h);
	double Quantity();
};

class PropellantRotationalSwitch: public RotationalSwitch {
public:
	PropellantRotationalSwitch();
	void SetSource(int num, PropellantSource *s);
	PropellantSource *GetSource();

protected:
	PropellantSource *sources[16];
};

class RCSQuantityMeter : public MeterSwitch {
public:
	RCSQuantityMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s, Saturn *v);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	PropellantRotationalSwitch *source;
	SURFHANDLE NeedleSurface;
	Saturn *our_vessel;
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

class SaturnRoundMeter : public MeterSwitch {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s);

protected:
	HPEN Pen0;
	HPEN Pen1;
	Saturn *Sat;

	void DrawNeedle (SURFHANDLE surf, int x, int y, double rad, double angle);
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
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, ToggleSwitch *h2oqtyindswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	ToggleSwitch *H2oQtyIndSwitch;
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
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newValue);

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
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newValue);
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

class SaturnACVoltMeter: public SaturnRoundMeter {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, PowerStateRotationalSwitch *acindicatorswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

	SURFHANDLE FrameSurface;

protected:
	double AdjustForPower(double val) { return val; } // These are always powered by definition.
	PowerStateRotationalSwitch *ACIndicatorSwitch;
};

class SaturnDCVoltMeter: public SaturnRoundMeter {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, PowerStateRotationalSwitch *dcindicatorswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

	SURFHANDLE FrameSurface;

protected:
	double AdjustForPower(double val) { return val; } // These are always powered by definition.
	PowerStateRotationalSwitch *DCIndicatorSwitch;
};

class SaturnDCAmpMeter: public SaturnRoundMeter {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, PowerStateRotationalSwitch *dcindicatorswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

	SURFHANDLE FrameSurface;

protected:
	double AdjustForPower(double val) { return val; } // These are always powered by definition.
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

class SaturnFuelCellConnectSwitch: public SaturnThreePosSwitch {
public:
	SaturnFuelCellConnectSwitch() { fuelCell = 0; dcBusController = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s, int fc, DCBusController *dcController);
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

protected:
	void CheckFuelCell(int s);

	int fuelCell;
	DCBusController *dcBusController;
};

