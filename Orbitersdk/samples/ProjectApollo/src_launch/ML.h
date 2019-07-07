/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ML vessel

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

#pragma once

#include "IUUmbilicalInterface.h"
#include "TSMUmbilicalInterface.h"

class Saturn;
class IUUmbilical;
class TSMUmbilical;
class IUSV_ESE;
class SIC_ESE;

const double ML_SIC_INTERTANK_ARM_CONNECTING_SPEED = 1.0 / 300.0;
const double ML_SIC_INTERTANK_ARM_RETRACT_SPEED = 1.0 / 13.0;
const double ML_SIC_FORWARD_ARM_CONNECTING_SPEED = 1.0 / 300.0;
const double ML_SIC_FORWARD_ARM_RETRACT_SPEED = 1.0 / 5.2;
const double ML_SWINGARM_CONNECTING_SPEED = 1.0 / 200.0;
const double ML_SWINGARM_RETRACT_SPEED = 1.0 / 5.0;
const double ML_TAIL_SERVICE_MAST_CONNECTING_SPEED = 1.0 / 100.0;
const double ML_TAIL_SERVICE_MAST_RETRACT_SPEED = 1.0 / 2.0;

///
/// \ingroup Ground
///
class ML: public VESSEL2, public IUUmbilicalInterface, public TSMUmbilicalInterface {

public:
	ML(OBJHANDLE hObj, int fmodel);
	virtual ~ML();

	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkPostCreation();
	void clbkLoadStateEx(FILEHANDLE scn, void *status);
	void clbkSaveState(FILEHANDLE scn);
	int clbkConsumeDirectKey(char *kstate);
	int clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkPostStep(double simt, double simdt, double mjd);

	virtual void SetVABBuildState();
	virtual void SetVABReadyState();
	virtual bool Detach();
	virtual bool Attach();
	virtual bool IsInVAB(); 

	// ML/IU Interface
	bool ESEGetCommandVehicleLiftoffIndicationInhibit();
	bool ESEGetSICOutboardEnginesCantInhibit();
	bool ESEGetSICOutboardEnginesCantSimulate();
	bool ESEGetAutoAbortInhibit();
	bool ESEGetGSEOverrateSimulate();
	bool ESEGetEDSPowerInhibit();
	bool ESEPadAbortRequest();
	bool ESEGetThrustOKIndicateEnableInhibitA();
	bool ESEGetThrustOKIndicateEnableInhibitB();
	bool ESEEDSLiftoffInhibitA();
	bool ESEEDSLiftoffInhibitB();
	bool ESEAutoAbortSimulate();
	bool ESEGetSIBurnModeSubstitute();
	bool ESEGetGuidanceReferenceRelease();

	//ML/S-IC Interface
	bool ESEGetSICThrustOKSimulate(int eng);

protected:
	bool firstTimestepDone;
	int meshindexML;
	bool moveToPadA;
	bool moveToPadB;
	bool moveToVab;
	bool moveLVToPadA;
	bool moveLVToPadB;
	double touchdownPointHeight;
	char LVName[256];
	SoundLib soundlib;
	OBJHANDLE hLV;
	int state;

	UINT craneAnim;
	UINT cmarmAnim;
	UINT s1cintertankarmAnim;
	UINT s1cforwardarmAnim;
	UINT swingarmAnim;
	UINT mastAnim;
	double craneProc;
	double cmarmProc;
	AnimState s1cintertankarmState;
	AnimState s1cforwardarmState;
	AnimState swingarmState;
	AnimState mastState;

	PSTREAM_HANDLE liftoffStream[2];
	double liftoffStreamLevel;

	Saturn *sat;
	IUUmbilical *IuUmb;
	TSMUmbilical *TSMUmb;
	IUSV_ESE *IuESE;
	SIC_ESE *SICESE;

	void DoFirstTimestep();
	double GetDistanceTo(double lon, double lat);
	void SetTouchdownPointHeight(double height);
	void DefineAnimations();

	bool CutoffInterlock();
	bool Commit();

	void MobileLauncherComputer(int mdo, bool on = true);

	void TerminalCountdownSequencer(double MissionTime);

	int TCSSequence;
	bool Hold;
};
