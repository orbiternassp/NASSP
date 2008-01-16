#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "checklistController.h"

using namespace std;
//Todo: Verify
ChecklistController::ChecklistController()
{
	// This SHOULD be final implementation.  Any further initializing should be handled between init and load.
	initCalled = false;
	init(false);
}
//Todo: Verify
ChecklistController::~ChecklistController()
{
}
//todo: Verify
bool ChecklistController::getChecklistItem(ChecklistItem* input)
{
// Disable signed/unsigned mismatch warning, we're being careful.
#pragma warning ( push )
#pragma warning ( disable: 4018 )
	// Catcher, are we being told to initialize a checklist group?
	if (input->group > -1)
	{
		if(!spawnCheck(input->group,false))
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
// Reenable warnings
#pragma warning ( pop )
}
//todo: Verify
vector <ChecklistGroup> *ChecklistController::getChecklistList()
{
	groups_manual = vector<ChecklistGroup>();
	for (int i = 0; i < groups.size(); i++)
	{
		if (groups[i].manualSelect)
			groups_manual.push_back(groups[i]);
	}
	return &groups_manual;
}
//todo: Verify
bool ChecklistController::failChecklistItem(ChecklistItem* input)
{
	//Verify integrity of input.
	if (input->group != active.program.group)
		return false;
	if (input->index >= active.set.size())
		return false;

	
	if (active.set[input->index] == *input)
	{
		active.set[input->index].failed = true;
	}
	else
		return false;

	int failure = active.set[input->index].failEvent;
	iterate();
	// Special handling for failure branching.
	if (failure > -1)
	{
		spawnCheck(failure,true);
	}
	return true;
}
//todo: Verify
bool ChecklistController::completeChecklistItem(ChecklistItem* input)
{
// Disable signed/unsigned mismatch warning, we're being careful.
#pragma warning ( push )
#pragma warning ( disable: 4018 )
	// Input Verification.
	if (input->group != active.program.group)
		return false;	//Simply not allowed.
	if (input->index >= active.set.size())
			return false; //catcher.

	if (active.set[input->index] == *input) // Just in case
	{
		active.set[input->index].complete = true; //This hopefully won't create chaos.
	}
	else
		return false;
	
	iterate();
	return true;
// Reenable signed/unsigned mismatch checking
#pragma warning ( pop )
}
//todo: Verify
bool ChecklistController::autoComplete(bool input)
{
	bool temp = complete;
	complete = input;
	return temp;
}
//todo: Verify
bool ChecklistController::autoComplete()
{
	return complete;
}
//todo: Verify
bool ChecklistController::autoExecute(bool set)
{
	bool temp = autoexecute;
	autoexecute = set;
	return temp;
}
//todo: Verify
bool ChecklistController::autoExecute()
{
	return autoexecute;
}
//todo: Verify
bool ChecklistController::spawnCheck(int group, bool fail, bool automagic)
{
// Code to tell the compiler to shut up about signed/unsigned mismatch.  Code is implemented in a way to verify no problems.
#pragma warning( push )
#pragma warning( disable : 4018 )

	// Verify input integrity:
	if (group < 0) //ERROR, HOW DID THIS HAPPEN?
		return false;
	if (groups.size() <= group) // group exists?
		return false;
	if (!fail && !groups[group].manualSelect && !automagic) // group was selected manually which isn't allowed.
		return false;
	if (!fail && !groups[group].autoSelect && automagic) // group was selected automatically which isn't allowed.
		return false;

	// Special case, we are launching a failure checklist.
	if(fail)
	{
		action.push_front(active);
		active = ChecklistContainer(groups[group],file,groups);
	}
	//Possible cases:
	//1) There is no group active - COMPLETE
	if (active.program.group == -1)
	{
		// Simply spawn new checklist.
		active = ChecklistContainer(groups[group],file,groups);
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
		action.push_back(ChecklistContainer(groups[group],file,groups));
		return true;
	}
	//4b) Group is not automagic and not essential - COMPLETE
	if (!automagic && !groups[group].essential)
	{
		//4ba) Active group is essential - COMPLETE
		if (active.program.essential)
		{
			// We don't override active, so just jump in the front of the line.
			action.push_front(ChecklistContainer(groups[group],file,groups));
			return true;
		}
		//4bb) Active group is not essential - COMPLETE
		else
		{
			// We are overriding active, so put active back in the front of the line.
			action.push_front(active);
			// Now create a new active for our group.
			active = ChecklistContainer(groups[group],file,groups);
			return true;
		}
	}
	//4c) Group is essential NOTICE: This disregards automagic completely as it ALWAYS goes to the front. - COMPLETE
	//4ca) Active group is essential NOTICE: Don't override an essential group. - COMPLETE
	if (active.program.essential)
	{
		// We NEVER override an essential group, so we'll just jump in the front of the line.
		action.push_front(ChecklistContainer(groups[group],file,groups));
		return true;
	}
	//4cb) Active group is not essential - COMPLETE
	else
	{
		// Active is not essential, so push it out of the way (to the front of the line).
		action.push_front(active);
		// Spawn new active with our group.
		active = ChecklistContainer(groups[group],file,groups);
		return true;
	}
	// Somehow, we were unable to spawn the checklist.  Getting this return with known valid data represents a bug.
	return false;
// reenable warning 4018 concerning signed/unsigned mismatch.
#pragma warning ( pop )
}
//Todo: Verify
bool ChecklistController::linktoVessel(VESSEL *vessel)
{
	return conn.ConnectToVessel(vessel);
}
//Todo: Verify
void ChecklistController::iterate()
{
	while (active.sequence != active.set.end() && (active.sequence->complete || active.sequence->failed))
	{
		//Turn off the flashing.
		conn.SetFlashing(active.sequence->item,false);
		//Move to next element.
		active.sequence ++;
		//Did we reach the end of the chekclist?  If so, load next.
		while (active.program.group != -1 && active.sequence == active.set.end())
		{
			// Set to empty checklist item
			active = ChecklistContainer();
			// Determine if we have another program to switch to.
			if (action.size() > 0)
			{
				// Switch to it and remove it from the que
				active = action[0];
				action.pop_front();
			}
		}
		// Keep checking our status.
	}
}
//todo: fully implement save/load functions. (Top level save complete)
void ChecklistController::save(FILEHANDLE scn)
{
	oapiWriteScenario_string(scn,ChecklistControllerStartString,"");
	oapiWriteScenario_string(scn,"FILE",FileName);
	oapiWriteScenario_int(scn,"AUTO",autoexecute);
	oapiWriteScenario_string(scn,ChecklistControllerEndString,"");
}
//todo: fully implement save/load functions. (Top level load complete)
void ChecklistController::load(FILEHANDLE scn)
{
	char *line, buffer[100];
	buffer[0] = 0;
	line="";
	while (strnicmp(line,ChecklistControllerEndString,strlen(ChecklistControllerEndString)))
	{
		oapiReadScenario_nextline(scn,line);
		if (!strnicmp(line,"FILE",4))
		{
			char temp[100];
			temp[0] = 0;
			strcpy(temp,line+5);
			init(temp);
		}
		if (!strnicmp(line,"AUTO",4))
		{
			sscanf(line+4, "%i", &autoexecute);
		}
	}
	init();
}
//todo: assess and implement.
void ChecklistController::clbkTimestep(double missionTime)
{
	lastMissionTime = missionTime;
}
//todo: any further non-file specific initialization needed.
bool ChecklistController::init(bool final)
{
	if (initCalled)
		return false;
	complete = true;
	initCalled = final;
	lastMissionTime = 0;
	FileName[0] = 0;
	if (!final)
		autoexecute = false;

	return true;
}
//todo: Verify
bool ChecklistController::init(char *checkFile)
{
	if(!init(true))
		return false;
	strcpy(FileName,checkFile);
	//Steps: Load file, Parse Groups page.
	if (!file.Load(checkFile))
	{
		if (!file.Load("Doc\\Project Apollo - NASSP\\Check List\\DefaultCheck.xls"))
			return false;
		else
			strcpy(FileName,"Doc\\Project Apollo - NASSP\\Check List\\DefaultCheck.xls");
	}

	char temp[100];
	temp[0] = 0;
	BasicExcelWorksheet* sheet = file.GetWorksheet("GROUPS");
	BasicExcelCell* cell = sheet->Cell(1,0);
	int i = 1;
	ChecklistGroup t;
	while (cell->GetString() != 0)
	{
		strcpy(temp,cell->GetString());
		// Already initialized to the first cell's contents.
		strcpy(t.name,temp);
		// Move to time column.
		cell = sheet->Cell(i,1);
		// should be stored as a double
		t.time = cell->GetDouble();
		// Move to relative event column.
		cell = sheet->Cell(i,2);
		// Should be stored as a string.
		t.relativeEvent = NO_TIME_DEF;
		if (cell->GetString())
		{
			strcpy(temp,cell->GetString());
			//Relative event selection
			if (!strnicmp(temp,"MISSION_TIME",12))
				t.relativeEvent = MISSION_TIME;
		}
		// Move to auto select column.
		cell = sheet->Cell(i,3);
		// Convert from integer to boolean is natively supported.
		t.autoSelect = (cell->GetInteger() != 0);
		// Move to manual select column.
		cell = sheet->Cell(i,4);
		t.manualSelect = (cell->GetInteger() != 0);
		// Move to essential column.
		cell = sheet->Cell(i,5);
		t.essential = (cell->GetInteger() != 0);
		//Put it into the vector.
		t.group = groups.size();
		groups.push_back(t);
		t = ChecklistGroup();
		i++;
		cell = sheet->Cell(i,0);
	}

	return true;
}
//todo: Verify
void ChecklistController::init()
{
	init("Doc\\Project Apollo - NASSP\\Check List\\DefaultCheck.xls");
}