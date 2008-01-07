#ifndef __checklistController_h
#define __checklistController_h
#include <vector>
#include <deque>
#include <set>
using namespace std;
struct ChecklistGroup
{
// -------------------------------------------------------------
// index defined at runtime.  Use this in a ChecklistItem struct
// to start the checklist operation of that group.
// -------------------------------------------------------------
	int group;
// -------------------------------------------------------------
// Time at which this group should be executed. (available at 
// this time)
// -------------------------------------------------------------
	int time;
// -------------------------------------------------------------
// Event relative to which the time triggers.
// -------------------------------------------------------------
	int relativeEvent;
// -------------------------------------------------------------
// Group can be manually selected from the MFD.  All checklists
// normally visible to the MFD have this set to true.
// -------------------------------------------------------------
	bool manualSelect;
// -------------------------------------------------------------
// Checklist can be automatically selected.  If both this and
// manualSelect are false, the checklist will never be displayed.
// -------------------------------------------------------------
	bool autoSelect;
// -------------------------------------------------------------
// Checklist group must be executed and takes priority in auto
// selection of checklist items over the existing item.
// Should this be true, then at the defined time, it will begin
// replacing the present checklist with the checklist group.
// It will internally stack up the old group and return once the
// group is done.
	bool essential;
// -------------------------------------------------------------
// Name of the group as should be displayed on the checklist.
// -------------------------------------------------------------
	char *Name;
};
struct ChecklistItem
{
// -------------------------------------------------------------
// index defined dynaimcally at runtime.  All available
// checklists have a group id retrieved from the ChecklistGroup
// structure.
// -------------------------------------------------------------
	int group;
// -------------------------------------------------------------
// defined linearly.  This index always defines the item its 
// same number of places from the beginning of the checklist 
// group
// -------------------------------------------------------------
	int index;
// -------------------------------------------------------------
// Either MET (if no relativeEvent is defined) or time before or
// after the event.
// -------------------------------------------------------------
	int time;
// -------------------------------------------------------------
// Event from the "events" enum listing the many managed events
// of this system.
// -------------------------------------------------------------
	int relativeEvent;
// -------------------------------------------------------------
// group id to go to in the event of a failure of this check step.
// -------------------------------------------------------------
	ChecklistGroup failEvent;
// -------------------------------------------------------------
// Text to display describing the checklist.
// -------------------------------------------------------------
	char *text;
// -------------------------------------------------------------
// extra text to display when the info button is pressed.
// -------------------------------------------------------------
	char *info;
// -------------------------------------------------------------
// define whether this checklist will happen automatically in
// quickstart mode
// -------------------------------------------------------------
	bool automatic;
// -------------------------------------------------------------
// reference to the panel switch that must be thrown, used to
// spawn reference box
// -------------------------------------------------------------
	char *item;
// -------------------------------------------------------------
// position the switch must be moved to.  Used for auto detect
// of checklist complete.
// -------------------------------------------------------------
	int position;
// -------------------------------------------------------------
// This is set to represent whether the step should be shown as
// already complete.
// -------------------------------------------------------------
	bool complete;
// -------------------------------------------------------------
// This is set to represent whether the step should be shown as
// failed.
// -------------------------------------------------------------
	bool failed;
// -------------------------------------------------------------
// This is an internal operator for the class.  The output should
// not be expected to be representative of true equality between
// the items, but rather is used for an internal check only.
// -------------------------------------------------------------
	bool operator==(ChecklistItem input);
};
struct ChecklistContainer
{
	ChecklistGroup group;
	vector<ChecklistItem> set;
	vector<ChecklistItem>::iterator sequence;
	ChecklistContainer(ChecklistGroup groupin);
	~ChecklistContainer();
};
// -------------------------------------------------------------
// Structure implementing the compare method required to allow
// For a sorted range of checklist groups.
// This allows for easy and efficient finding of the items desired.
// -------------------------------------------------------------
struct ChecklistGroupcompare
{
	bool operator()(const ChecklistGroup& lhs, const ChecklistGroup& rhs);
};
class ChecklistController
{
public:
// -------------------------------------------------------------
// Saturn* constructor.  Takes in a Saturn reference in order
// to automate checklist actions.  This is essential as we need
// to be able to automate from the file.
// -------------------------------------------------------------
	ChecklistController();
	~ChecklistController();
// -------------------------------------------------------------
// Pass in a ChecklistItem with group and index initialized.
// If group is initialized to a valid group, then it is assumed
// that you want to start that group, and the returned item will
// be the first item in the group.
// If group is initialized to -1 then index becomes the index from
// the "current" checklist item to return.
// In the event that an essential checklist is running, a selected
// checklist will not actually start running but get placed at the
// top of the stack to be run when the essential checklist is done.
	void getChecklistItem(ChecklistItem*);
// -------------------------------------------------------------
// Gets list of available groups at this time.  Return is an 
// array that should be deleted once it is not needed.  
// (Maintaining a single dynamic array updated to this list 
// every 10 seconds or so is probably a good implementation).
// -------------------------------------------------------------
	ChecklistGroup *getChecklistList();
// -------------------------------------------------------------
// This checklist item is failed.  In order to properly handle
// this situation, we need to actually tell the controller instead
// of just jumping away.  The controller will tell us where to go.
// -------------------------------------------------------------
	bool failChecklistItem(ChecklistItem*);
// -------------------------------------------------------------
// This checklist item is complete.  In the event it is the item
// currently in the iterator, it will simply move the iterator.
// In the event it is a checklist down the road, it will simply 
// display completed.
// -------------------------------------------------------------
	bool completeChecklistItem(ChecklistItem*);
// -------------------------------------------------------------
// This allows setting of the autoComplete function which 
// automatically detects if a step is already complete and marks
// it as such, hiding it if it's already at the top of the list.
// returns old state.
// -------------------------------------------------------------
	bool autoComplete(bool);
	void clbkTimestep(double missionTime);
// -------------------------------------------------------------
// This method does default (empty) initialization.  Can be 
// called to indicate that the Checklist controller should do 
// nothing.  WARNING:  once this is called, you cannot call any
// other init function!
// -------------------------------------------------------------
	void init();
// -------------------------------------------------------------
// This method initializes any Saturn for any mission.  Uses a
// custom checklist file where the checkFile parameter is the
// proper path to the file.  WARNING:  once this is called, you
// cannot call any other init function!
// -------------------------------------------------------------
	void init(char *checkFile);
protected:
private:
	//Auto complete flag.  If true, automatically complete the checklist.
	bool complete;
	//flag to prevent re-init of module.  Only the constructor can call 
	//init without locking the class in the given state.
	bool initCalled;
	//This is a "stackable queue" implemented by using either the push
	//front/push back functions to allow checklist groups to be lined
	//up for execution.
	deque<ChecklistContainer> action;
	//The active checklist group.
	ChecklistContainer *active;
	//The "sorted list" of all available checklist groups.
	set<ChecklistGroup,ChecklistGroupcompare> groups;
	//The mission time at which the controller was last called.
	double lastMissionTime;
	//This is where actual "default" init happens.  Only the constructor calls this with false.
	bool init(bool final);
};
#endif