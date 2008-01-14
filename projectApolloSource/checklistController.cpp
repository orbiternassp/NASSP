#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "checklistController.h"

using namespace std;
//Todo: none
ChecklistController::ChecklistController()
{
	// This SHOULD be final implementation.  Any further initializing should be handled between init and load.
	initCalled = false;
	init(false);
}
//Todo: none
ChecklistController::~ChecklistController()
{
}
//todo: Double check, returns false if failed to init new program OR if no data to return.
bool ChecklistController::getChecklistItem(ChecklistItem* input)
{
	/// Requesting to begin a new group.
	if (input->group != -1)
	{
		/// Try to spawn the group.  Failure here does NOT mean no group active, but that the MFD should evaluate why it failed.
		if (!spawnCheck(input->group))
		{
			input->index = -1;
			return false;
		}
	}	
	/// Make sure we have something to execute:
	if (active.program.group == -1)
	{
		input->index = -1;
		return false;
	}
	/// If a specific index is requested:
	if (input->index != -1)
	{
		vector<ChecklistItem>::iterator temp(&(*(active.sequence)),&(active.set));
		for (int i = 0; i < input->index; i++, temp++);
		*input = *temp;
	}
	/// Just want the first index:
	else
	{
		*input = *(active.sequence);
	}
	return true;
}
//todo: Implement checklistgroup collection and return.
bool ChecklistController::getChecklistList(vector<ChecklistGroup>*)
{
	return false;
}
//todo: Implement failure action code.
bool ChecklistController::failChecklistItem(ChecklistItem* input)
{
	return false;
}
//todo: Implement stepping code.
bool ChecklistController::completeChecklistItem(ChecklistItem* input)
{
	return false;
}
//todo: implement with auto-complete part of timestep function.
bool ChecklistController::autoComplete(bool)
{
	return false;
}
//todo: asses and implement.
void ChecklistController::clbkTimestep(double missionTime)
{
	lastMissionTime = missionTime;
}
//todo: any further non-file specific initialization needed.  Remove "dummy" group.
bool ChecklistController::init(bool final)
{
	if (initCalled)
		return false;
	complete = false;
	initCalled = final;
	action = stackedQue<ChecklistContainer>();
	active = ChecklistContainer();
	groups = vector<ChecklistGroup>();
	lastMissionTime = 0;
	//Remove before distro
	{
		ChecklistGroup temp;
		temp.autoSelect = true;
		temp.essential = false;
		temp.group = 0;
		temp.manualSelect = true;
		temp.name = "Test Group";
		temp.relativeEvent = MISSION_TIME;
		temp.time = -3840;
		groups.push_back(temp);
	}
	return true;
	//Todo: Define the understanding that this is going to be a "inactive" controller.
}
//todo: All file specific initialization.
bool ChecklistController::init(char *checkFile)
{
	if(!init(true))
		return false;
	//Todo: Initial loading of complete list of groups.
	//Todo: Initial loading of proper group to active.
	//Todo: figure out how to save state... potentially spit out the queue in fifo order
	//Todo: Appearance: Checklist <group name> <index>
	//Todo: Figure out how to know when a specific event has happened.

	return true;
}
//todo: believed complete, verify.
void ChecklistController::init()
{
	init(true);
}
//todo: fully implement save/load functions.
void ChecklistController::save(FILEHANDLE scn)
{
	return;
	oapiWriteScenario_string(scn,ChecklistControllerStartString,"");
	oapiWriteScenario_string(scn,ChecklistControllerEndString,"");
}
//todo: fully implement save/load functions.
void ChecklistController::load(FILEHANDLE scn)
{
	char *line, buffer[100];
	line = "";
	while (strnicmp(line,ChecklistControllerEndString,strlen(ChecklistControllerEndString)))
	{
		oapiReadScenario_nextline(scn,line);
	}
}
//todo: presumed complete.
bool ChecklistController::autoExecute(bool set)
{
	bool temp = autoexecute;
	autoexecute = set;
	return temp;
}
//todo: presumed complete.
bool ChecklistController::autoExecute()
{
	return autoexecute;
}
//todo: confirm properly checks that checklist item is not active.  Complete spawn work.
bool ChecklistController::spawnCheck(int group)
{
	/// Check that we actually have programs available.
	if (groups.size() == 0)
		return false;
	/// If there is nothing yet active:
	if (active.program.group == -1)
	{
		vector<ChecklistGroup>::iterator iter = groups.begin();
		while (iter != groups.end() && iter->group != group)
		{
			iter++;
		}
		/// Stopped iterating through, did we find a viable program?
		if (iter->group == group && iter->manualSelect)
		{
			active = ChecklistContainer(*iter);
			active.sequence = active.set.begin();
		}
		else
			return false;
		return true;
	}
	/// See if this program is currently active
	if (active.program.group == group)
	{
		return true;
	}
	/// See if it's in que, if so, bring it to the front.
	int location = -1;
	for (int i = 0; i < action.size(); i++)
	{
		if (action[i].program.group = group)
			location = i;
	}
	///Program is already active, now to "reload" it.
	if (location != -1)
	{
		///First, check if it is allowed to override active group.
		if (!active.program.essential)
		{
			///Allowed to override active group, so stack active group.
			action.stack(active);
			active = ChecklistContainer(action[location]);
		}
		else
		{
			///Not allowed to override active group so move to top of stack.
			action.stack(*(new ChecklistContainer(action[location])));
		}
		/// Once we have taken care of loading it, we need to remove it from it's earlier point in the que.
		location++;
		for (int i = location; i < action.size(); i++)
		{
			action[i] = action[i+1];
		}
		action.pop_back();
		return true;
	}
	// End of block to identify if we have this programming running already and load it into the first slot.
	/// Find out if we have this program available to start.
	vector<ChecklistGroup>::iterator iter = groups.begin();
	while (iter != groups.end() && iter->group != group)
	{
		iter++;
	}
	/// Stopped iterating through, did we find a viable program?
	if (iter->group == group && iter->manualSelect)
	{
		if (active.program.essential)
		{
			action.stack(ChecklistContainer(*iter));
		}
		else
		{
			action.stack(active);
			active = ChecklistContainer(*iter);
		}
		return true;
	}
	return false;	//Program not found.
}