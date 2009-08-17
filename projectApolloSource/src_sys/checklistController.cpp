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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.1  2009/02/18 23:21:48  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.22  2008/07/13 17:47:12  tschachim
  *	Rearranged realism levels, merged Standard and Quickstart Mode.
  *	
  *	Revision 1.21  2008/06/17 16:39:07  tschachim
  *	Moved prime crew ingress to T-2h40min, bugfixes checklists.
  *	
  *	Revision 1.20  2008/05/27 14:53:30  tschachim
  *	New event LAST_ITEM_RELATIVE, bugfixes.
  *	
  *	Revision 1.19  2008/05/24 17:30:42  tschachim
  *	Bugfixes, new flash toggle.
  *	
  *	Revision 1.18  2008/04/11 12:00:53  tschachim
  *	Cleanup of the checklist events.
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  **************************************************************************/


// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "nasspdefs.h"
#include "checklistController.h"

//Code to make the compiler shut up.
#pragma warning ( push )
#pragma warning ( disable:4018 )

using namespace std;
// Todo: Verify
ChecklistController::ChecklistController(SoundLib sound)
{
	initCalled = false;
	soundLib = sound;
	FileName[0] = 0;
	init(false);
}
// Todo: Verify
ChecklistController::~ChecklistController()
{
}
// Todo: Verify
bool ChecklistController::getChecklistItem(ChecklistItem* input)
{
	if (input->group != -1)
	{
		spawnCheck(input->group,false);
	}

	if (active.program.group == -1)
		return false;

	if (input->index != -1)
	{
		if (input->index + active.sequence->index >= active.set.size())
			return false;
		*input = active.set[active.sequence->index + input->index];
		return true;
	}
	*input = *active.sequence;
	return true;
}
// Todo: Verify
vector<ChecklistGroup> *ChecklistController::getChecklistList()
{
	groups_manual.clear();
	for (int i = 0; i < groups.size(); i++)
		if (groups[i].manualSelect)
			groups_manual.push_back(groups[i]);

	return &groups_manual;
}
// Todo: Verify
bool ChecklistController::failChecklistItem(ChecklistItem* input)
{
	int failure = input->failEvent;

	if (input->group != active.program.group)
		return false;
	if (input->index >= active.set.size())
		return false;
	active.set[input->index].status = FAILED;
	conn.SetFlashing(active.set[input->index].item,false);
	iterate();

	if (failure != -1)
		return spawnCheck(failure,true);
	return true;
}
// Todo: Verify
bool ChecklistController::completeChecklistItem(ChecklistItem* input)
{
	if (input->group != active.program.group)
		return false;
	if (input->index >= active.set.size())
		return false;

	active.set[input->index].status = COMPLETE;
	conn.SetFlashing(active.set[input->index].item,false);
	lastItemTime = lastMissionTime;
	iterate();
	return true;
}
// Todo: Verify
bool ChecklistController::autoComplete(bool input)
{
	bool temp = complete;
	complete = input;
	return temp;
}
// Todo: Verify
bool ChecklistController::autoComplete()
{
	return complete;
}
// Todo: Verify
bool inline ChecklistController::init()
{
	return init(DefaultChecklistFile);
}
// Todo: Verify
bool ChecklistController::init(char *checkFile)
{
	if (!init(true))
		return false;

	if (*checkFile == '\0') {
		if (!file.Load(DefaultChecklistFile)) {
			return false;
		}
	} else if (!file.Load(checkFile)) {
		if (!file.Load(DefaultChecklistFile)) {
			return false;
		}
	}

	BasicExcelWorksheet* sheet;
	vector<BasicExcelCell> cells;
	ChecklistGroup temp;

	sheet = file.GetWorksheet("GROUPS");
	if (sheet)
	{
		for (int i = 1; i < sheet->GetTotalRows(); i++)
		{
			for (int ii = 0; ii < 9/*Number of columns in accepted sheet*/; ii++)
				cells.push_back(*sheet->Cell(i,ii));
			temp.init(cells);
			temp.group = groups.size();
			groups.push_back(temp);
			temp = ChecklistGroup();
			cells = vector<BasicExcelCell>();
		}
	}
	return true;
}
// Todo: Verify
void ChecklistController::save(FILEHANDLE scn)
{
	int i = 0;

	oapiWriteScenario_string(scn, ChecklistControllerStartString, "");
	oapiWriteScenario_string(scn, "FILE", FileName);
	oapiWriteScenario_int(scn, "COMPLETE", (complete ? 1 : 0));
	oapiWriteScenario_int(scn, "FLASHING", (flashing ? 1 : 0));
	oapiWriteScenario_float(scn, "LASTITEMTIME", lastItemTime);

	if (active.program.group != -1)
		active.save(scn);
	for (i = 0; i < action.size(); i++)
		action[i].save(scn);

	for (i = 0; i < groups.size(); i++)
		if (groups[i].called)
			groups[i].save(scn);

	oapiWriteScenario_string(scn,ChecklistControllerEndString,"");
}
// Todo: Verify
void ChecklistController::load(FILEHANDLE scn)
{
	char *line,buffer[100];
	line = "";
	buffer[0] = 0;
	while (strnicmp(line,ChecklistControllerEndString,strlen(ChecklistControllerEndString)))
	{
		bool found = false;
		if (!found && !strnicmp(line,"FILE",4))
		{
			strncpy(FileName,line+5,100);
			init(FileName);
			found = true;
		}
		if (!found && !strnicmp(line,"COMPLETE",8))
		{
			int i;
			sscanf(line+8,"%d",&i);
			complete = (i != 0);
			found = true;
		}
		if (!found && !strnicmp(line,"FLASHING",8))
		{
			int i;
			sscanf(line+8,"%d",&i);
			flashing = (i != 0);
			found = true;
		}
		if (!found && !strnicmp(line, "LASTITEMTIME", 12))
		{
			float flt = 0;
            sscanf (line + 12, "%f", &flt);
			lastItemTime = flt;
			found = true;
		}
		if (!found && !strnicmp(line,ChecklistContainerStartString,strlen(ChecklistContainerStartString)))
		{
			ChecklistContainer temp;
			temp.load(scn,*this);
			if (active.program.group == -1)
				active = temp;
			else
				action.push_back(temp);
			found = true;
		}
		if (!found && !strnicmp(line,ChecklistGroupStartString,strlen(ChecklistGroupStartString)))
		{
			oapiReadScenario_nextline(scn,line);
			int i;
			if (!strnicmp(line,"INDEX",5))
			{
				sscanf(line+5,"%d",&i);
				groups[i].load(scn);
			}
			found = true;
		}

		oapiReadScenario_nextline(scn,line);
	}
	init();
}
// Todo: Verify
bool ChecklistController::autoExecute(bool input)
{
	bool temp = autoexecute;
	autoexecute = input;
	return temp;
}
// Todo: Verify
bool ChecklistController::autoExecuteSlow(bool input)
{
	bool temp = autoexecuteSlow;
	autoexecuteSlow = input;
	return temp;
}
// Todo: Verify
bool ChecklistController::autoExecute()
{
	return autoexecute;
}
// Todo: Verify
bool ChecklistController::linktoVessel(VESSEL *vessel)
{
	return conn.ConnectToVessel(vessel);
}

// Todo: Verify
bool ChecklistController::init(bool input)
{
	if (initCalled)
		return false;
	initCalled = input;
	complete = true;
	flashing = false;
	lastMissionTime = MINUS_INFINITY;
	lastItemTime = MINUS_INFINITY;
	autoexecute = false;
	autoexecuteSlow = false;
	playSound = false;

	return true;
}
// Todo: Verify
bool ChecklistController::spawnCheck(int group,bool failed,bool automagic)
{
	// verify integrity of input.
	if (group == -1)
		return false; //How we got here, I don't know.
	if (group >= groups.size())
		return false; //Shouldn't get here, but just to be certain.
	if (!failed && !automagic && !groups[group].manualSelect) //Not a failure spawn, not auto spawn, and not permitted to manual spawn
		return false;
	if (!failed && automagic && !groups[group].autoSelect) //Not a failure spawn, auto, not permitted to be auto
		return false;
	if (active.program.group == group) //Already active
		return true;
	
	// Begin checking if it's queued up.
	int location = -1;
	for (int i = 0; i < action.size(); i++)
		if (action[i].program.group == group)
			location = i;

	// automagic, not essential, leave be
	// otherwise process.
	if (location > -1 && automagic && !action[location].program.essential)
		return true;
	ChecklistContainer temp;
	if (location > -1)
	{
		temp = action[location];
		for (int i = location; i < action.size() - 1; i++)
			action[i] = action[i+1];
		action.pop_back();
	}
	else
	{
		temp = ChecklistContainer(groups[group],*this,lastMissionTime);
	}

	// Use temp.
	// Case 1: failed
	if (failed)
	{
		// case a: active is empty.
		if (active.program.group == -1)
		{
			active = temp;
			return true;
		}
		// case b: Otherwise.
		else
		{
			// Stop flashing just in case 
			conn.SetFlashing(active.sequence->item, false);
			action.push_front(active);
			active = temp;
			return true;
		}
	}
	// Case 2: automagic
	if (automagic)
	{
		// case a: active is empty
		if (active.program.group == -1)
		{
			active = temp;
			return true;
		}
		// case b: non-essential
		if (!temp.program.essential)
		{
			action.push_back(temp);
			return true;
		}
		// case c: essential
		if (temp.program.essential)
		{
			// case c1:Active is essential
			if (active.program.essential)
			{
				action.push_front(temp);
				return true;
			}
			// case c2:Active is not essential
			else
			{
				// Stop flashing just in case 
				conn.SetFlashing(active.sequence->item, false);
				action.push_front(active);
				active = temp;
				return true;
			}
		}
	}
	// Case 3: manual
	if (!automagic)
	{
		// case a: active is empty
		if (active.program.group == -1)
		{
			active = temp;
			return true;
		}
		// case b: active is essential
		if (active.program.essential)
		{
			action.push_front(temp);
			return true;
		}
		// case c: active is not essential.
		if (!active.program.essential)
		{
			// Stop flashing just in case 
			conn.SetFlashing(active.sequence->item, false);
			action.push_front(active);
			active = temp;
			return true;
		}
	}
	return false; //Catcher
}
// Todo: Verify
void ChecklistController::iterate()
{
	while (active.program.group != -1 && active.sequence->status != PENDING)
	{
		if (active.sequence->index == (active.set.size() - 1))
		{
			if (action.size() > 0)
			{
				active = action[0];
				action.pop_front();
			}
			else
				active = ChecklistContainer();
		}
		else
			active.sequence ++;
	}
}
// Todo: Verify
void ChecklistController::timestep(double missiontime, SaturnEvents eventController)
{
	// Play Sound
	if (playSound)
	{
		if (checkSound.play())
			playSound = false;
	}

	// Flashing
	if (active.program.group != -1 && active.sequence->checkExec(lastMissionTime, active.startTime, lastItemTime, eventController)) {
		conn.SetFlashing(active.sequence->item, flashing);
	}

	// Even on "non executing" timesteps, we want to allow to complete at least one checklist item
	if (complete)
	{
		if (active.program.group != -1 && conn.GetState(active.sequence->item) == active.sequence->position &&
			active.sequence->checkExec(missiontime, active.startTime, lastItemTime, eventController)) {
			completeChecklistItem(&(*active.sequence));
		}
	}

	//Exit if less than one second
	if (missiontime < (lastMissionTime + 1.0))
		return;
	if (autoexecute && autoexecuteSlow && missiontime < (lastMissionTime + 2.0))
		return;
	lastMissionTime = missiontime;

	//Check for groups needing spawn
	for (int i = 0; i < groups.size(); i++)
	{
		if (groups[i].autoSelect && !groups[i].called)
		{
			if(groups[i].checkExec(lastMissionTime,eventController))
			{
				spawnCheck(i,false,true);
			}
		}
	}

	//Do checklist items
	if (autoexecute)
	{
		if (autoexecuteSlow) {
			if (active.program.group != -1 && active.sequence->checkExec(lastMissionTime, active.startTime, lastItemTime, eventController))
			{
				if (active.sequence->position < 0)
					completeChecklistItem(&(*active.sequence));
				else if (conn.SetState(active.sequence->item,active.sequence->position))
					completeChecklistItem(&(*active.sequence));
			}
		} else {
			while (active.program.group != -1 && active.sequence->checkExec(lastMissionTime, active.startTime, lastItemTime, eventController))
			{
				if (active.sequence->position < 0)
					completeChecklistItem(&(*active.sequence));
				else if (conn.SetState(active.sequence->item,active.sequence->position))
					completeChecklistItem(&(*active.sequence));
			}
		}
	}

	//Check for complete checklist items
	if (complete)
	{
		while (active.program.group != -1 && conn.GetState(active.sequence->item) == active.sequence->position &&
			   active.sequence->checkExec(lastMissionTime, active.startTime, lastItemTime, eventController)) {
			completeChecklistItem(&(*active.sequence));
		}
	}
}

// Todo: Verify
char *ChecklistController::activeName()
{
	if (active.program.group == -1)
		return 0;
	else
		return active.program.name;
}
// Todo: Verify
bool ChecklistController::retrieveChecklistContainer(ChecklistContainer *input)
{
	if (input->program.group >= groups.size())
		return false;
	if (!groups[input->program.group].manualSelect) //Only manually selectable checklists should be reviewable.
		return false;
	*input = ChecklistContainer(groups[input->program.group],*this,lastMissionTime,true);
	return true;
}
/*
bool complete;
bool initCalled;
deque<ChecklistContainer> action;
ChecklistContainer active;
vector<ChecklistGroup> groups;
double lastMissionTime;
bool autoexecute;
MFDConnector conn;
char FileName[100];
BasicExcel file;
vector<ChecklistGroup> groups_manual;
SoundLib soundLib;
Sound checkSound;
bool playSound;

complete
initCalled
action
active
groups
lastMissionTime
autoexecute
conn
FileName
file
groups_manual
soundLib
checkSound
playSound
*/
#pragma warning ( pop )