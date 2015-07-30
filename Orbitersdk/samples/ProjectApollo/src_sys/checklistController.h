/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2008 

  Checklist controller

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


#ifndef __checklistController_h
#define __checklistController_h

//
// Disable annoying warning for compilers older 
// than Microsoft Visual Studio Version 2003 
//

#if defined(_MSC_VER) && (_MSC_VER < 1300) 
#pragma warning(disable : 4786 ) 
#endif

// Disable 4018 warning for the extent of this header file.
#pragma warning ( push )
#pragma warning ( disable:4018 )

#include <vector>
#include <deque>
#include <string>
// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbiterSDK.h"
#include "nasspdefs.h"
#include "connector.h"
#include "BasicExcelVC6.hpp"
#include "soundlib.h"
using namespace std;
using namespace YExcel;

#define ChecklistControllerStartString "<checklist>"
#define ChecklistControllerEndString "</checklist>"
#define SaturnEventStartString "SaturnEvents"
#define SaturnEventEndString "SaturnEvents END"
#define ChecklistItemStartString "<item>"
#define ChecklistItemEndString "</item>"
#define ChecklistGroupStartString "<group>"
#define ChecklistGroupEndString "</group>"
#define ChecklistContainerStartString "<container>"
#define ChecklistContainerEndString "</container>"

#define DefaultChecklistFile "Doc\\Project Apollo - NASSP\\Check List\\Mission checklist\\Default Checklist.xls"

struct SaturnEvents;
/// -------------------------------------------------------------
/// Listing of available events to spawn a checklist off of.
/// -------------------------------------------------------------
enum RelativeEvent
{
	HIDDEN_DELAY = -4, /// < like LAST_ITEM_RELATIVE, but not visible in the MFD.  WARNING! Not valid for groups.
	LAST_ITEM_RELATIVE = -3, /// < check item time is relative to the completion of the last item.  WARNING! Not valid for groups.
	CHECKLIST_RELATIVE = -2, /// < check item time is relative to the beginning of the checklist.  WARNING! Not valid for groups.
	MISSION_TIME = -1, /// < check item happens at specific MJD WARNING! could cause unexpected results if items after this become scheduled to operate before it based on a checklist event starting late.  Should primarily only be used for primary checklists!
	NO_TIME_DEF = 0, /// < check item happens when it is reached NOTE: in auto mode, this item will inherit the state of last time-relative checklist item before it.  Groups with this definition will NOT be automated at all.

	BACKUP_CREW_PRELAUNCH,
	PRIME_CREW_PRELAUNCH,
	SECOND_STAGE_STAGING,
	TOWER_JETTISON,
	SIVB_STAGE_STAGING,
	EARTH_ORBIT_INSERTION,
	TLI,
	CSM_LV_SEPARATION,
	CSM_LV_SEPARATION_DONE,
	PAYLOAD_EXTRACTION,
	CM_SM_SEPARATION,
	CM_SM_SEPARATION_DONE,
	SPLASHDOWN,
};
RelativeEvent checkEvent(const char*, bool Group=false);
enum Status
{
	FAILED = -1,
	PENDING = 0,
	COMPLETE = 1
};
/// -------------------------------------------------------------
/// A "checklist group" which defines an available checklist
/// "program"
/// -------------------------------------------------------------
struct ChecklistGroup
{
/// -------------------------------------------------------------
/// Default constructor.
/// -------------------------------------------------------------
	ChecklistGroup()
	{
		group = -1;
		time = 0;
		deadline = 0;
		relativeEvent = NO_TIME_DEF;
		manualSelect = false;
		autoSelect = false;
		essential = false;
		name[0] = 0;
		heading[0] = 0;
		autoSlow = false;
		soundFile[0] = 0;
		called = false;
	}
/// -------------------------------------------------------------
/// Load this checklist group up from a file.
/// -------------------------------------------------------------
	void init(vector<BasicExcelCell> &);
/// -------------------------------------------------------------
/// Scenario handling.
/// -------------------------------------------------------------
	void load(FILEHANDLE);
	void save(FILEHANDLE);
/// -------------------------------------------------------------
/// Check whether this group should be loaded in this timestep
/// -------------------------------------------------------------
	bool checkExec(double,SaturnEvents &);
/// -------------------------------------------------------------
/// index defined at runtime.  Use this in a ChecklistItem struct
/// to start the checklist operation of that group.
/// -------------------------------------------------------------
	int group;
/// -------------------------------------------------------------
/// Time at which this group should be executed. (available at 
/// this time)
/// -------------------------------------------------------------
	double time;
/// -------------------------------------------------------------
/// Time after which this group should not be executed 
/// automatically.
/// -------------------------------------------------------------
	double deadline;
/// -------------------------------------------------------------
/// Event relative to which the time triggers.
/// -------------------------------------------------------------
	RelativeEvent relativeEvent;
/// -------------------------------------------------------------
/// Group can be manually selected from the MFD.  All checklists
/// normally visible to the MFD have this set to true.
/// -------------------------------------------------------------
	bool manualSelect;
/// -------------------------------------------------------------
/// Checklist can be automatically selected.  If both this and
/// manualSelect are false, the checklist will never be displayed.
/// -------------------------------------------------------------
	bool autoSelect;
/// -------------------------------------------------------------
/// Checklist group must be executed and takes priority in auto
/// selection of checklist items over the existing item.
/// Should this be true, then at the defined time, it will begin
/// replacing the present checklist with the checklist group.
/// It will internally stack up the old group and return once the
/// group is done.
	bool essential;
/// -------------------------------------------------------------
/// Name of the group as should be displayed on the checklist.
/// -------------------------------------------------------------
	char name[100];
/// -------------------------------------------------------------
/// Heading
/// -------------------------------------------------------------
	char heading[100];
/// -------------------------------------------------------------
/// Auto slow when this group is spawned.
/// -------------------------------------------------------------
	bool autoSlow;
/// -------------------------------------------------------------
/// name of sound file.
/// -------------------------------------------------------------
	char soundFile[100];
/// -------------------------------------------------------------
/// Boolean defining if this has been called yet.
/// -------------------------------------------------------------
	bool called;
};

struct DSKYChecklistItem
{
	DSKYChecklistItem()
	{
		key[0] = 0;
	};

	void init(char *k);

	char key[10];
/// -------------------------------------------------------------
/// reference to the panel switch that must be thrown, used to
/// spawn reference box
/// -------------------------------------------------------------
	char item[100];
	char item2[100];
};


/// -------------------------------------------------------------
/// An individual element in a checklist program.  These elements
/// can be triggered as complete either by the user or the auto
/// complete code.  In quickstart mode, the element will be
/// switched automatically if enabled.
/// -------------------------------------------------------------
class MFDConnector;

struct ChecklistItem
{
/// -------------------------------------------------------------
/// Default constructor, does a proper "empty" construction
/// -------------------------------------------------------------
	ChecklistItem();
/// -------------------------------------------------------------
/// Load ChecklistItem from excel file
/// -------------------------------------------------------------
	void init(vector<BasicExcelCell> &, const vector<ChecklistGroup> &);
/// -------------------------------------------------------------
/// Scenario load/save.
/// -------------------------------------------------------------
	void load(FILEHANDLE);
	void save(FILEHANDLE);

	bool iterate(MFDConnector *conn, bool autoexec);

	void setFlashing(MFDConnector *conn, bool flashing);
	
	double getAutoexecuteSlowDelay(MFDConnector *conn);

	double checkIterate(MFDConnector *conn);

/// -------------------------------------------------------------
/// Check whether this item should be executed in this timestep
/// -------------------------------------------------------------
	bool checkExec(double, double, double, SaturnEvents &, bool autoexecuteAllItemsAutomatic);
/// -------------------------------------------------------------
/// index defined dynaimcally at runtime.  All available
/// checklists have a group id retrieved from the ChecklistGroup
/// structure.
/// -------------------------------------------------------------
	int group;
/// -------------------------------------------------------------
/// defined linearly.  This index always defines the item its 
/// same number of places from the beginning of the checklist 
/// group
/// -------------------------------------------------------------
	int index;
/// -------------------------------------------------------------
/// Either MET (if no relativeEvent is defined) or time before or
/// after the event.
/// -------------------------------------------------------------
	double time;
/// -------------------------------------------------------------
/// Event from the "events" enum listing the many managed events
/// of this system.
/// -------------------------------------------------------------
	RelativeEvent relativeEvent;
/// -------------------------------------------------------------
/// group id to go to in the event of a failure of this check step.
/// always null when received by the MFD.
/// -------------------------------------------------------------
	int failGroup;

	int callGroup;
/// -------------------------------------------------------------
/// Text to display describing the checklist.
/// -------------------------------------------------------------
	char text[100];
/// -------------------------------------------------------------
/// Panel number or description
/// -------------------------------------------------------------
	char panel[100];
/// -------------------------------------------------------------
/// Heading
/// -------------------------------------------------------------
	char heading1[100];
/// -------------------------------------------------------------
/// Subheading
/// -------------------------------------------------------------
	char heading2[100];
/// -------------------------------------------------------------
/// extra text to display when the info button is pressed.
/// -------------------------------------------------------------
	char info[300];
/// -------------------------------------------------------------
/// define whether this checklist will happen automatically in
/// quickstart mode
/// -------------------------------------------------------------
	bool automatic;
/// -------------------------------------------------------------
/// reference to the panel switch that must be thrown, used to
/// spawn reference box
/// -------------------------------------------------------------
	char item[100];
/// -------------------------------------------------------------
/// position the switch must be moved to.  Used for auto detect
/// of checklist complete.
/// -------------------------------------------------------------
	int position;
	bool guard;
	bool hold;
	bool lineFeed;

/// -------------------------------------------------------------
/// This is the status of the class.  It is saved and loaded when
/// this item is active or pending.
/// -------------------------------------------------------------
	Status status;
	
	vector<DSKYChecklistItem> dskyItemsSet;
	int dskyIndex;
	int dskyNo;
	bool dskyPressed;

/// -------------------------------------------------------------
/// This is an internal operator for the class.  The output should
/// not be expected to be representative of true equality between
/// the items, but rather is used for an internal check only.
/// -------------------------------------------------------------
	bool operator==(ChecklistItem);
};
/// -------------------------------------------------------------
/// Structure containing an active checklist "program"  This
/// structure contains the general group definition as well as a
/// complete list of the group's items.  It also contains an 
/// iterator that sits on the "current" checklist item for this
/// "program"
/// -------------------------------------------------------------
struct ChecklistContainer
{
/// -------------------------------------------------------------
/// Default Constructor.  Initializes an empty container.
/// -------------------------------------------------------------
	ChecklistContainer();
/// -------------------------------------------------------------
/// Constructor, requires a group to initialize from.
/// -------------------------------------------------------------
	ChecklistContainer(const ChecklistGroup &, ChecklistController &,double, bool dontspawn = false);
/// -------------------------------------------------------------
/// Copy Constructor.
/// -------------------------------------------------------------
	ChecklistContainer(const ChecklistContainer &);
/// -------------------------------------------------------------
/// Assignment operator.
/// -------------------------------------------------------------
	void operator=(const ChecklistContainer &);
/// -------------------------------------------------------------
/// Scenario functions
/// -------------------------------------------------------------
	void save(FILEHANDLE);
	void load(FILEHANDLE, ChecklistController &);
/// -------------------------------------------------------------
/// Checklist Group that this "program" is based on.
/// -------------------------------------------------------------
	ChecklistGroup program;
/// -------------------------------------------------------------
/// The list of elements in this "program"
/// -------------------------------------------------------------
	vector<ChecklistItem> set;
/// -------------------------------------------------------------
/// An iterator that rests on the current step of the program.
/// -------------------------------------------------------------
	vector<ChecklistItem>::iterator sequence;
/// -------------------------------------------------------------
/// The time this checklist was started.
/// -------------------------------------------------------------
	double startTime;
private:
/// -------------------------------------------------------------
/// Initializer for the initializer.
/// -------------------------------------------------------------
	void initSet(const ChecklistGroup &,vector<ChecklistItem> &, ChecklistController &);
};
/// -------------------------------------------------------------
/// Structure that maintains Saturn related event times.
/// -------------------------------------------------------------
struct SaturnEvents
{
	SaturnEvents();
	void save(FILEHANDLE scn);
	void load(FILEHANDLE scn);

	double BACKUP_CREW_PRELAUNCH;	// Time of backup crew ingress and prelaunch checks.
	double PRIME_CREW_PRELAUNCH;	// Time of prime crew ingress and cabin closeout.
	double SECOND_STAGE_STAGING;	// Time of S-IC/S-II staging (or S-IB/S-IVB staging in case of the Saturn 1B).
	double TOWER_JETTISON;			// Time at which the Launch Escape Tower was jettisoned.
	double SIVB_STAGE_STAGING;		// Time of S-II/S-IVB staging (or simultaneous with tower jettison in case of the Saturn 1B).
	double EARTH_ORBIT_INSERTION;	// Time of Earth Parking Orbit (EPO) insertion.
	double TLI;						// Time at which the TLI burn begins.
	double CSM_LV_SEPARATION;		// Time at which the CSM/LV separation sequence starts 
	double CSM_LV_SEPARATION_DONE;	// Time at which the CSM/LV separation occured 
	/// \todo The S-IVB has no checklist controller yet
	double PAYLOAD_EXTRACTION;		// Time at which the payload is removed from the SIVB 
	double CM_SM_SEPARATION;		// Time at which the CM/SM separation sequence starts 
	double CM_SM_SEPARATION_DONE;	// Time at which the CM/SM separation occured 
	double SPLASHDOWN;				// Time of splashdown.
};

#ifndef _PA_MFDCONNECTOR_H
#include "MFDconnector.h" //Has to be done here because fails further up.

/// -------------------------------------------------------------
/// This is the actual controller.  It exists once in each vessel
/// that implements the need to operate a checklist.  It keeps a 
/// set of available checklist groups that can be loaded to be
/// an active checklist "program"
/// -------------------------------------------------------------
class ChecklistController
{
public:
	ChecklistController(SoundLib);
	~ChecklistController();
/// -------------------------------------------------------------
/// Pass in a ChecklistItem with group and index initialized.
/// If group is initialized to a valid group, then it is assumed
/// that you want to start that group, and the returned item will
/// be the first item in the group.
/// If group is initialized to -1 then index becomes the index from
/// the "current" checklist item to return.
/// In the event that an essential checklist is running, a selected
/// checklist will not actually start running but get placed at the
/// top of the stack to be run when the essential checklist is done.
	ChecklistItem *getChecklistItem(int group, int index);
/// -------------------------------------------------------------
/// Basic accessor for the list of available checklist items.
/// Access only when needed.  Not responsible for maintaining the
/// data.
/// -------------------------------------------------------------
	vector<ChecklistGroup> *getChecklistList();
/// -------------------------------------------------------------
/// This checklist item is failed.  In order to properly handle
/// this situation, we need to actually tell the controller instead
/// of just jumping away.  The controller will tell us where to go.
/// -------------------------------------------------------------
	bool failChecklistItem(ChecklistItem*);
/// -------------------------------------------------------------
/// This checklist item is complete.  In the event it is the item
/// currently in the iterator, it will simply move the iterator.
/// In the event it is a checklist down the road, it will simply 
/// display completed.
/// -------------------------------------------------------------
	bool completeChecklistItem(ChecklistItem*);
/// -------------------------------------------------------------
/// This allows setting of the autoComplete function which 
/// automatically detects if a step is already complete and marks
/// it as such, hiding it if it's already at the top of the list.
/// returns old state.
/// -------------------------------------------------------------
	bool autoComplete(bool);
	bool autoComplete();
/// -------------------------------------------------------------
/// This method does default (empty) initialization.  Can be 
/// called to indicate that the Checklist controller should do 
/// nothing.  WARNING:  once this is called, you cannot call any
/// other init function!  Should NOT be called except in the case
/// you want to override the functions loaded by the scenario.
/// IE you want to use an unmanned flight.
/// -------------------------------------------------------------
	bool inline init();
/// -------------------------------------------------------------
/// This method initializes any vessel for any mission.  Uses a
/// custom checklist file where the checkFile parameter is the
/// proper path to the file.  Otherwise, pass in null to indicate
/// should use default file.
/// WARNING:  once this is called, you cannot call any other init
/// function!
/// -------------------------------------------------------------
	bool init(char *checkFile);
/// -------------------------------------------------------------
/// called to save checklistController state.
/// -------------------------------------------------------------
	void save(FILEHANDLE scn);
/// -------------------------------------------------------------
/// called to load checklistController state.
/// -------------------------------------------------------------
	void load(FILEHANDLE scn);
/// -------------------------------------------------------------
/// Set the auto execute setting.  This is used by the vessel
/// to set the launchpad configuration options setting
/// -------------------------------------------------------------
	bool autoExecute(bool set);
/// -------------------------------------------------------------
/// Get the auto execute setting.
/// -------------------------------------------------------------
	bool autoExecute();
/// -------------------------------------------------------------
/// Set the auto execute slow setting.  This is used by the vessel
/// to set the launchpad configuration options setting
/// -------------------------------------------------------------
	bool autoExecuteSlow(bool input);
/// -------------------------------------------------------------
/// Do manual items like automatic (for Quickstart Mode)
/// -------------------------------------------------------------
	void autoExecuteAllItemsAutomatic(bool input) { autoexecuteAllItemsAutomatic = input; };
/// -------------------------------------------------------------
/// Used to link checklist controller to a vessel, required to
/// Allow automated checklists as well as automatic checklist
/// selection and automatic completion detection.
/// -------------------------------------------------------------
	bool linktoVessel(VESSEL *);
/// -------------------------------------------------------------
/// Timestep function.  If autoexecute is on, completes checklist
/// items that are due.  Updates internal timer.  Only executes
/// once per second (returns immediately other calls each second)
/// Additionally processes "complete" steps.
/// -------------------------------------------------------------
	void timestep(double missiontime, SaturnEvents eventController);
/// -------------------------------------------------------------
/// Returns the title of the active checklist.
/// -------------------------------------------------------------
	char *activeName();
/// -------------------------------------------------------------
/// Returns an entire checklist
/// -------------------------------------------------------------
	bool retrieveChecklistContainer(ChecklistContainer *);

	bool getFlashing() { return flashing; };

	void setFlashing(bool f) { flashing = f; };

private:
	/// Auto complete flag.  If true, automatically complete the checklist.
	bool complete;
	///flag to prevent re-init of module.  Only the constructor can call 
	///init without locking the class in the given state.
	bool initCalled;
	///This is a "stackable queue" implemented by using either the push
	///front/push back functions to allow checklist groups to be lined
	///up for execution.
	deque<ChecklistContainer> action;
	///The active checklist group.
	ChecklistContainer active;
	///The mission time at which the controller was last called.
	double lastMissionTime;
	///This is where actual "default" init happens.  Only the constructor calls this with false.
	bool init(bool);
	///This determines whether or not the checklist gets auto executed.
	bool autoexecute;
	///This determines the checklist execution speed.
	bool autoexecuteSlow;
	double autoexecuteSlowDelay;
	// Do manual items like automatic (for Quickstart Mode)
	bool autoexecuteAllItemsAutomatic;
	///Used to spawn new "program"
	bool spawnCheck(int, bool, bool automagic = false);
	bool doSpawnCheck(int, bool, bool automagic = false);
	///Connector to the panel
	MFDConnector conn;
	/// Used to move forward through the elements.
	void iterate();
	///The file reference.
	char FileName[100];
	///A temporary, frequently regenerated list of all availabe manually selectable checklists.
	vector<ChecklistGroup> groups_manual;
	/// flashing the current switch
	bool flashing;
	/// Mission time when the last item was completed 
	double lastItemTime;

	bool waitForCompletion;
	
	bool iterateChecklistItem(double missiontime, SaturnEvents eventController, bool autoexec = false);

	bool isDSKYChecklistItem();

protected:	
	/// Access to the vessels sound handler
	SoundLib soundLib;
	/// Sound that needs to be played.
	Sound checkSound;
	/// Whether we have a sound cued up to be played.
	bool playSound;
	/// The actual file.
	BasicExcel file;
	///The list of all available checklist groups.
	vector<ChecklistGroup> groups;
public:
	friend struct ChecklistContainer;
};

//Reenable 4018 warning
#pragma warning ( pop )
#endif
#endif
