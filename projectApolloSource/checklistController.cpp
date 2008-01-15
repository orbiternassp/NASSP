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
//todo: implement
bool ChecklistController::getChecklistItem(ChecklistItem* input)
{
	// Catcher, are we being told to initialize a checklist group?
	if (input->group > -1)
	{
		if(!spawnCheck(input->group))
			return false; // If we failed to initialize the group, stop here and FAIL.
	}

	// We are preparing to output data.
	// Make sure we have something to output.
	if (active.program.group == -1)
		return false; //No active program.  Don't display anything.

	if (input->index > -1) //Are we being asked for a specific input item?  Or just any old item?
	{
		if (input->index+active.sequence->index >= active.set.size())
			return false; // We are being asked for an index that doesn't exist.  Can happen quite often.
		vector<ChecklistItem>::iterator temp = active.set.begin(); //Create new iterator on the vector.
		temp += active.sequence->index; // Move the iterator to the location of sequence.
		temp += input->index; // Move it further forward according to the given index.
		*input = *temp; //Set input accordingly.
		return true; //Return.
	}
	else
	{
		*input = *active.sequence; //We just want the first item.
		return true; //Return.
	}
	return false; //Catcher case, should never be hit.
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
//todo: any further non-file specific initialization needed.
//cleanup: remove unnecessary debug-only constructs
bool ChecklistController::init(bool final)
{
	if (initCalled)
		return false;
	complete = false;
	initCalled = final;
	lastMissionTime = 0;
#ifdef _DEBUG //Todo: remove once file loading code is implemented.
	{
		ChecklistGroup temp;
		temp.autoSelect = true;
		temp.essential = false;
		temp.group = 0;
		temp.manualSelect = true;
		strcpy(temp.name,"Test Group 1");
		temp.relativeEvent = MISSION_TIME;
		temp.time = -3840;
		groups.push_back(temp);
		temp.group = 1;
		strcpy(temp.name,"Test Group 2");
		temp.relativeEvent = NO_TIME_DEF;
		temp.autoSelect = false;
		groups.push_back(temp);
		temp.group = 2;
		strcpy(temp.name,"Test Group 3");
		groups.push_back(temp);
	}
#endif
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
	buffer[0] = 0;
	line="";
	while (strnicmp(line,ChecklistControllerEndString,strlen(ChecklistControllerEndString)))
	{
		oapiReadScenario_nextline(scn,line);
	}
}
//todo: implement
bool ChecklistController::autoExecute(bool set)
{
	bool temp = autoexecute;
	autoexecute = set;
	return temp;
}
//todo: implement
bool ChecklistController::autoExecute()
{
	return autoexecute;
}
//todo: Verify - Especially Verify after checklist completion code is complete.
bool ChecklistController::spawnCheck(int group, bool automagic)
{
// Code to tell the compiler to shut up about signed/unsigned mismatch.  Code is implemented in a way to verify no problems.
#pragma warning( push )
#pragma warning( disable : 4018 )

	// Verify input integrity:
	if (group < 0) //ERROR, HOW DID THIS HAPPEN?
		return false;
	if (groups.size() <= group) // group exists?
		return false;
	if (!groups[group].manualSelect && !automagic) // group was selected manually which isn't allowed.
		return false;
	if (!groups[group].autoSelect && automagic) // group was selected automatically which isn't allowed.
		return false;

	//Possible cases:
	//1) There is no group active - COMPLETE
	if (active.program.group == -1)
	{
		// Simply spawn new checklist.
		active = ChecklistContainer(groups[group]);
		return true;
	}
	//2) group is currently active - COMPLETE
	if (active.program.group == group)
	{
		// No need to do anything, we are trying to start the active checklist.
		return true;
	}
	//3) group is not currently active but is on standby - COMPLETE
	//Checking standby elements to see if it's there.
	// init location which will serve dual purpose.  It will tell us if we found it, and where it is.
	int location = -1;
	for (int i=0; i < action.size(); i++)
	{
		if (action[i].program.group == group) // If we actually found it, save it's location.
			location = i;
	}
	// Do this if it exists.
	if (location > -1)
	{
		//3a) active group is essential - COMPLETE
		if (active.program.essential)
		{
			// Create temporary container.
			ChecklistContainer temp(action[location]);
			// Now that we have our data, remove it from the que.
			for (int i = location; i < action.size()-1; i++)
			{
				action[i] = action[i+1]; // Copy the next item into the current slot.
			}
			action.pop_back(); //Should not be a problem, this should be a duplicate at this point.
			action.push_front(temp); //Put our new program at the very beginning.
			return true;
		}
		//3b) active group is not essential. - COMPLETE
		else
		{
			//We're replacing active, so put it back on the que.
			action.push_front(active);
			//That last action moved everything down one, save the location.
			location++;
			//Load up our program.
			active = action[location];
			// Now that we have our data, remove it from the que.
			for (int i = location; i < action.size()-1; i++)
			{
				action[i] = action[i+1]; // Move items after new program up one
			}
			action.pop_back(); //Should not be a problem, this should be a duplicate at this point.
			return true;
		}
	}
	//4) Group needs to be initialized. - DEFAULT!
	//4a) Group is automagic and not essential - COMPLETE
	if (automagic && !groups[group].essential)
	{
		// It's auto and not essential.  We'll get to it when we get to it, throw it at the back of the que.
		action.push_back(ChecklistContainer(groups[group]));
		return true;
	}
	//4b) Group is not automagic and not essential - COMPLETE
	if (!automagic && !groups[group].essential)
	{
		//4ba) Active group is essential - COMPLETE
		if (active.program.essential)
		{
			// We don't override active, so just jump in the front of the line.
			action.push_front(ChecklistContainer(groups[group]));
			return true;
		}
		//4bb) Active group is not essential - COMPLETE
		else
		{
			// We are overriding active, so put active back in the front of the line.
			action.push_front(active);
			// Now create a new active for our group.
			active = ChecklistContainer(groups[group]);
			return true;
		}
	}
	//4c) Group is essential NOTICE: This disregards automagic completely as it ALWAYS goes to the front. - COMPLETE
	//4ca) Active group is essential NOTICE: Don't override an essential group. - COMPLETE
	if (active.program.essential)
	{
		// We NEVER override an essential group, so we'll just jump in the front of the line.
		action.push_front(ChecklistContainer(groups[group]));
		return true;
	}
	//4cb) Active group is not essential - COMPLETE
	else
	{
		// Active is not essential, so push it out of the way (to the front of the line).
		action.push_front(active);
		// Spawn new active with our group.
		active = ChecklistContainer(groups[group]);
		return true;
	}
	// Somehow, we were unable to spawn the checklist.  Getting this return with known valid data represents a bug.
	return false;
// reenable warning 4018 concerning signed/unsigned mismatch.
#pragma warning ( pop )
}