/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: LEM-specific switches

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

class LEM;
class LEM_ECAch;
class LEM_INV;

class LEMThreePosSwitch : public ThreePosSwitch {
public:
	LEMThreePosSwitch() { lem = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM *s);

protected:
	LEM *lem;
};

class LEMMissionTimerSwitch : public LEMThreePosSwitch {
public:
	LEMMissionTimerSwitch() { lem = 0; sw = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM *s, int id);
	//bool CheckMouseClick(int event, int mx, int my);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	LEM *lem;
	int sw;
};

class LEMInverterSwitch: public LEMThreePosSwitch {
public:
	LEMInverterSwitch() { inv1 = NULL; inv2 = NULL; };
class LEM_ECA;	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM *s, LEM_INV *lem_inv_1, LEM_INV *lem_inv_2);
	//bool CheckMouseClick(int event, int mx, int my);
	virtual bool SwitchTo(int newState, bool dontspring = false);
	bool ChangeState(int newState);

protected:
	void CheckValve(int s);
	LEM_INV *inv1;
	LEM_INV *inv2;
};

class LGCThrusterPairSwitch : public LEMThreePosSwitch {
public:
	LGCThrusterPairSwitch();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM *l, int bit);
	virtual bool SwitchTo(int newState, bool dontspring = false);
protected:
	int inputbit;
};

// This is a CB like any other, except it lies about current across itself.
class LEMVoltCB: public CircuitBrakerSwitch {
	double Current();
};

// Meters
class LEMRoundMeter : public RoundMeter {
public:
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, LEM *s);

protected:
	LEM *lem;
};

class LEMDCVoltMeter: public LEMRoundMeter {
public:
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, LEM *s, SURFHANDLE frameSurface);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

	SURFHANDLE FrameSurface;

protected:
	double AdjustForPower(double val) { return val; } // These are always powered by definition.	
};

class LEMDCAmMeter: public LEMRoundMeter {
public:
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, LEM *s, SURFHANDLE frameSurface);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

	SURFHANDLE FrameSurface;

protected:
	double AdjustForPower(double val) { return val; } // These are always powered by definition.	
};

class LMSuitTempMeter : public CurvedMeter {
public:
	LMSuitTempMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMCabinTempMeter : public CurvedMeter {
public:
	LMCabinTempMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMSuitPressMeter : public CurvedMeter {
public:
	LMSuitPressMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMCabinPressMeter : public CurvedMeter {
public:
	LMCabinPressMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMCO2Meter : public CurvedMeter {
public:
	LMCO2Meter();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMGlycolTempMeter : public CurvedMeter {
public:
	LMGlycolTempMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMGlycolPressMeter : public CurvedMeter {
public:
	LMGlycolPressMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMOxygenQtyMeter : public CurvedMeter {
public:
	LMOxygenQtyMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMWaterQtyMeter : public CurvedMeter {
public:
	LMWaterQtyMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMRCSATempInd : public CurvedMeter {
public:
	LMRCSATempInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMRCSBTempInd : public CurvedMeter {
public:
	LMRCSBTempInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMRCSAPressInd : public CurvedMeter {
public:
	LMRCSAPressInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMRCSBPressInd : public CurvedMeter {
public:
	LMRCSBPressInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMRCSAQtyInd : public CurvedMeter {
public:
	LMRCSAQtyInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class LMRCSBQtyInd : public CurvedMeter {
public:
	LMRCSBQtyInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class TempMonitorInd : public CurvedMeter {
public:
	TempMonitorInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class EngineThrustInd : public CurvedMeter {
public:
	EngineThrustInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class CommandedThrustInd : public CurvedMeter {
public:
	CommandedThrustInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class ThrustWeightInd : public LinearMeter {
public:
	ThrustWeightInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	double AdjustForPower(double val);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
};

class MainFuelTempInd : public CurvedMeter {
public:
	MainFuelTempInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s, ThreePosSwitch *temppressmonswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
	ThreePosSwitch *monswitch;
};

class MainFuelPressInd : public CurvedMeter {
public:
	MainFuelPressInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s, ThreePosSwitch *temppressmonswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
	ThreePosSwitch *monswitch;
};

class MainOxidizerTempInd : public CurvedMeter {
public:
	MainOxidizerTempInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s, ThreePosSwitch *temppressmonswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
	ThreePosSwitch *monswitch;
};

class MainOxidizerPressInd : public CurvedMeter {
public:
	MainOxidizerPressInd();
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *s, ThreePosSwitch *temppressmonswitch);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);

protected:
	LEM *lem;
	SURFHANDLE NeedleSurface;
	ThreePosSwitch *monswitch;
};

class EngineStartButton : public SimplePushSwitch {

public:
	EngineStartButton() {};
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int xoffset, int yoffset, ToggleSwitch* stopbutton, LEM *l);
	bool CheckMouseClick(int event, int mx, int my);
	bool CheckMouseClickVC(int event, VECTOR3 &p);
	bool Push();
	void DoDrawSwitch(SURFHANDLE DrawSurface);
	void DoDrawSwitchVC(SURFHANDLE surf, SURFHANDLE DrawSurface, int xTexMul = 1);
protected:
	ToggleSwitch* stopbutton;
	LEM *lem;
};

class EngineStopButton : public ToggleSwitch {

public:
	EngineStopButton() {};
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int xoffset, int yoffset, SimplePushSwitch* startbutton, LEM *l);
	bool CheckMouseClick(int event, int mx, int my);
	bool CheckMouseClickVC(int event, VECTOR3 &p);
	bool Push();
	void DoDrawSwitch(SURFHANDLE DrawSurface);
protected:
	SimplePushSwitch* startbutton;
	LEM *lem;
};

class LMAbortButton : public PushSwitch {
public:
	LMAbortButton() {};
	bool CheckMouseClick(int event, int mx, int my);
	bool CheckMouseClickVC(int event, VECTOR3 &p);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int xoffset, int yoffset, LEM *l);
	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState);
	bool SwitchTo(int newState);
protected:
	LEM *lem;
};

class LMAbortStageButton : public GuardedPushSwitch {
public:
	LMAbortStageButton();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int xoffset, int yoffset, LEM *l);
	bool CheckMouseClick(int event, int mx, int my);
	bool CheckMouseClickVC(int event, VECTOR3 &p);
	void DrawSwitch(SURFHANDLE DrawSurface);
protected:
	LEM *lem;
};

class LEMPanelOrdeal : public MeterSwitch {
public:
	void Init(SwitchRow &row, LEM *l);
	double QueryValue() { return 0; }
	void DoDrawSwitch(double v, SURFHANDLE drawSurface) {};

	int GetState();
	void SetState(int value);

protected:
	LEM *lem;
};

class RadarSignalStrengthAttenuator : public VoltageAttenuator {
public:
	RadarSignalStrengthAttenuator(char *i_name, double minIn, double maxIn, double minOut, double maxOut);
	void Init(LEM* l, RotationalSwitch *testmonitorselectorswitch, e_object *Instrum);
	double GetValue();
protected:
	LEM *lem;
	RotationalSwitch *TestMonitorRotarySwitch;
};

class LEMSteerableAntennaPitchMeter : public LEMRoundMeter {
public:
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, LEM *s, SURFHANDLE frameSurface);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);
protected:
	SURFHANDLE FrameSurface;
};

class LEMSteerableAntennaYawMeter : public LEMRoundMeter {
public:
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, LEM *s, SURFHANDLE frameSurface);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void OnPostStep(double SimT, double DeltaT, double MJD);
protected:
	SURFHANDLE FrameSurface;
};

class LEMSBandAntennaStrengthMeter : public LEMRoundMeter {
public:
	void Init(oapi::Pen *p0, oapi::Pen *p1, SwitchRow &row, LEM *s, SURFHANDLE frameSurface);
	double QueryValue();
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
protected:
	SURFHANDLE FrameSurface;
};

class LEMDPSValveTalkback : public IndicatorSwitch {
public:
	LEMDPSValveTalkback();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, DPSValve *v, bool failopen);
	int GetState();

protected:
	DPSValve *valve;
};

class LEMSCEATalkback : public IndicatorSwitch {
public:
	LEMSCEATalkback();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, SCEA_SolidStateSwitch *s, bool failopen = false);
	int GetState();

protected:
	SCEA_SolidStateSwitch * ssswitch;
};

class LEMDoubleSCEATalkback : public IndicatorSwitch {
public:
	LEMDoubleSCEATalkback();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, SCEA_SolidStateSwitch *s1, SCEA_SolidStateSwitch *s2);
	int GetState();

protected:
	SCEA_SolidStateSwitch * ssswitch1;
	SCEA_SolidStateSwitch * ssswitch2;
};

class LEMRCSQuadTalkback : public IndicatorSwitch {
public:
	LEMRCSQuadTalkback();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, SCEA_SolidStateSwitch *s, TCA_FlipFlop *tcaf);
	int GetState();
protected:
	SCEA_SolidStateSwitch * ssswitch;
	TCA_FlipFlop *tcaFailure;
};

class LEMDPSDigitalMeter : public MeterSwitch {
public:
	void Init(SURFHANDLE surf, SwitchRow &row, LEM *l);
	void InitVC(SURFHANDLE surf);
	void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	void DrawSwitchVC(int id, int event, SURFHANDLE surf);

protected:
	virtual double AdjustForPower(double val) { return val; };

	SURFHANDLE Digits, DigitsVC;
	LEM *lem;
};

class LEMDPSOxidPercentMeter : public LEMDPSDigitalMeter {
public:
	double QueryValue();
};

class LEMDPSFuelPercentMeter : public LEMDPSDigitalMeter {
public:
	double QueryValue();
};

class LEMDigitalHeliumPressureMeter : public MeterSwitch {
public:
	LEMDigitalHeliumPressureMeter();
	void Init(SURFHANDLE surf, SwitchRow &row, RotationalSwitch *s, LEM *l);
	void InitVC(SURFHANDLE surf);
	double QueryValue();
	virtual void DoDrawSwitch(double v, SURFHANDLE drawSurface);
	virtual void DrawSwitchVC(int id, int event, SURFHANDLE surf);

protected:
	virtual double AdjustForPower(double val) { return val; };

	RotationalSwitch *source;
	SURFHANDLE Digits, DigitsVC;
	LEM *lem;
};

class DEDAPushSwitch : public PushSwitch {
protected:
	virtual void DoDrawSwitch(SURFHANDLE DrawSurface);
};

class AscentO2RotationalSwitch : public RotationalSwitch
{
public:
	AscentO2RotationalSwitch();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, PushSwitch *InhibitSw, RotationalSwitch *DesO2Sw);
	virtual bool SwitchTo(int newValue);
protected:
	PushSwitch *InhibitSwitch;
	RotationalSwitch *DesO2Switch;
};

class LMSuitTempRotationalSwitch : public RotationalSwitch {
public:
	LMSuitTempRotationalSwitch() { 	Pipe = NULL; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, h_Pipe *p, h_Pipe *bp);
	virtual bool SwitchTo(int newValue);

protected:
	void CheckValve();

	h_Pipe *Pipe;
	h_Pipe *Bypass;
};

class LMLiquidGarmentCoolingRotationalSwitch : public RotationalSwitch {
public:
	LMLiquidGarmentCoolingRotationalSwitch() { Pipe = NULL; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, h_Pipe *hx, h_Pipe *p);
	virtual bool SwitchTo(int newValue);

protected:
	void CheckValve();

	h_Pipe *HX;
	h_Pipe *Pipe;
};

class LMForwardHatchHandle :public ToggleSwitch {
public:
	LMForwardHatchHandle();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row,  LEMForwardHatch *fh);
	virtual bool SwitchTo(int newState, bool dontspring = true);
protected:
	LEMForwardHatch *forwardHatch;
};

class LMOverheadHatchHandle :public ToggleSwitch {
public:
	LMOverheadHatchHandle();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEMOverheadHatch *oh);
	virtual bool SwitchTo(int newState, bool dontspring = true);
protected:
	LEMOverheadHatch *ovhdHatch;
};

class CDRCOASPowerSwitch : public LEMThreePosSwitch
{
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class LEM_CWEA;

class LEMMasterAlarmSwitch : public PushSwitch {

public:
	LEMMasterAlarmSwitch();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM_CWEA *c);
	void InitVC(SURFHANDLE surf);
	void DoDrawSwitch(SURFHANDLE DrawSurface);
	void DrawSwitchVC(int id, int event, SURFHANDLE surf);
	bool SwitchTo(int newState, bool dontspring = false);
protected:
	LEM_CWEA *cwea;
	SURFHANDLE switchsurfacevc;
};
