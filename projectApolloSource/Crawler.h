#include "orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "nasspsound.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"
#include "tracer.h"

class Crawler: public VESSEL2 {

public:
	Crawler(OBJHANDLE hObj, int fmodel);
	virtual ~Crawler();

	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkLoadStateEx(FILEHANDLE scn, void *status);
	void clbkSaveState(FILEHANDLE scn);
	int clbkConsumeDirectKey (char *kstate);
	void clbkPreStep (double simt, double simdt, double mjd);

protected:
	double velocity;
	double touchdownPointHeight;
	bool firstTimestepDone;

	bool keyAccelerate;
	bool keyBrake;
	bool keyLeft;
	bool keyRight;
	bool keyUp;
	bool keyDown;

	SoundLib soundlib;
	Sound soundEngine;

	void DoFirstTimestep();
	void SetTouchdownPoint();
};