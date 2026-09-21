/***************************************************************************
  This file is part of the GroundElapsedTime MFD addon for NASSP.

  This is a small, standalone Orbiter MFD (multi-function display) mode.
  Its only job is to show "Ground Elapsed Time" (GET, also called Mission
  Elapsed Time / MET) - i.e. how long it has been since a Saturn/Apollo
  mission lifted off - and to write that same value out to a plain text
  file once a second so another program outside Orbiter can use it too
  (for example, to play back an astronaut transcript in sync with the
  simulation).

  It is modeled directly on NASSP's own "ProjectApolloMFD" (see
  Orbitersdk/samples/ProjectApollo/src_mfd/ProjectApolloMFD.cpp, the
  "Draw mission time" section) and "ProjectApolloChecklistMFD" (see
  DisplayMissionElapsedTime() in ProjectApolloChecklistMFD.cpp). Rather
  than re-inventing the GET calculation, this addon calls the *same*
  internal NASSP vessel classes (Saturn, LEM, SIVB, Crawler, MCC) that
  ProjectApolloMFD does, so the number shown here always matches what
  NASSP itself considers the mission time to be.

  Beginner's note: in C++, a ".h" file like this one just *declares* what
  a class looks like (its name, and the names/types of its functions and
  member variables) without saying exactly what each function *does*.
  The "what it does" part lives in the matching ".cpp" file
  (GroundElapsedTimeMFD.cpp). Splitting code this way is a very common
  C++ convention.
  ***************************************************************************/

#ifndef __GROUNDELAPSEDTIMEMFD_H
#define __GROUNDELAPSEDTIMEMFD_H

///
/// GroundElapsedTimeMFD is our custom MFD mode. It inherits from Orbiter's
/// MFD2 base class, which is the standard way every Orbiter MFD (including
/// NASSP's own MFDs) is written. Inheriting means "GroundElapsedTimeMFD is
/// a kind of MFD2, with a few extra/overridden behaviors".
///
/// Note: the actual "which vessel is this, and what is its mission time"
/// logic lives in GroundElapsedTimeCommon.h/.cpp instead of here, because
/// that same logic is also needed by our file-export feature
/// (GroundElapsedTimeExport.cpp), which runs independently of whether this
/// MFD page happens to be on screen. Keeping it in one shared place means
/// the on-screen display and the exported file can never disagree.
///
class GroundElapsedTimeMFD : public MFD2
{
public:
	// The constructor: this code runs once, the moment the player selects
	// this MFD mode on a vessel. "w" and "h" are the pixel width/height of
	// the MFD screen, and "vessel" is a pointer to whichever vessel the MFD
	// is attached to (whatever the player is currently flying/viewing).
	GroundElapsedTimeMFD(DWORD w, DWORD h, VESSEL *vessel);

	// The destructor: runs once when the MFD is closed/switched away from.
	// We don't allocate anything that needs manual cleanup, but Orbiter's
	// MFD2 base class expects every MFD to declare one.
	~GroundElapsedTimeMFD();

	// Orbiter calls Update() many times per second while this MFD page is
	// the one actually being displayed on screen. This is where we compute
	// the current GET and draw it as text using the Sketchpad ("skp"),
	// which is Orbiter's simple 2D drawing API (text, lines, etc.).
	bool Update(oapi::Sketchpad *skp);

	// MsgProc is how Orbiter itself talks to our MFD mode - for example,
	// telling us "the player just opened this MFD mode, please create an
	// instance of your class". Every Orbiter MFD needs a function like
	// this; it must be declared "static" (meaning it isn't tied to any one
	// particular MFD instance) because Orbiter calls it before any
	// instance necessarily exists yet.
	static int MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

protected:
	// Screen size in pixels, remembered from the constructor so Update()
	// knows where to draw things.
	DWORD width;
	DWORD height;

	// The vessel this MFD instance is attached to (whatever the player is
	// currently flying/viewing). We re-check its type every time we draw,
	// via GroundElapsedTimeCommon's helper, rather than caching individual
	// Saturn*/LEM*/etc. pointers like ProjectApolloMFD does - our MFD only
	// ever needs one number (the GET), so there's no need for the extra
	// bookkeeping that a full-featured MFD like ProjectApolloMFD requires.
	VESSEL *ourVessel;
};

#endif // !__GROUNDELAPSEDTIMEMFD_H
