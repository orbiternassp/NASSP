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


// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "nasspdefs.h"
#include "nassputils.h"
#include "checklistController.h"
#include "saturn.h"
#include "LEM.h"

//Code to make the compiler shut up.
#pragma warning ( push )
#pragma warning ( disable:4018 )

using namespace nassp;
using namespace std;

// Stuff for the connector
// Cons
ChecklistDataInterface::ChecklistDataInterface() {
	type = CHECKLIST_DATA_INTERFACE;
	vessel = NULL;
}

// Other end
ChecklistDataInterface::ChecklistDataInterface(VESSEL *v) {
	type = CHECKLIST_DATA_INTERFACE;
	vessel = v;
}

// Destructor
ChecklistDataInterface::~ChecklistDataInterface() {

}

// Connect to vessel
bool ChecklistDataInterface::ConnectToVessel(VESSEL *v) {
	//
	// Disconnect in case we're already connected.
	//
	Disconnect();

	//
	// See if we can find the appropriate connector on the vessel.
	//	
	Connector *vsl = GetVesselConnector(v, VIRTUAL_CONNECTOR_PORT, type);
	
	//
	// Try to connect if we did.
	//
	if (vsl) {
		vessel = v;
		return ConnectTo(vsl);
	}
	return false;
}

bool ChecklistDataInterface::setVariable(int index, char * value) {
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = ChecklistDataInterface::CDI_SET_VARIABLE;
	cm.val1.iValue = index;
	cm.val2.pValue = value;

	if (SendMessage(cm)) 		{
		return cm.val1.bValue;
	}
	return false;
}

bool ChecklistDataInterface::getVariable(int index, char * value) {
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = ChecklistDataInterface::CDI_GET_VARIABLE;
	cm.val1.iValue = index;
	cm.val2.pValue = value;

	if (SendMessage(cm)) {
		return cm.val1.bValue;
	}
	return false;
}

bool ChecklistDataInterface::ReceiveMessage(Connector *from, ConnectorMessage &m) {
	int index = 0;
	char * value = NULL;
	int vessel_type = 0;

	//
	// Sanity checks
	//
	if (vessel == NULL) { 
		sprintf(oapiDebugString(), "vessel is null");
		return false;
	}
	if (m.destination != type) { return false; }	
	// It's ours then
	CDIMessageType messageType;
	messageType = (CDIMessageType)m.messageType;
	char * classname = vessel->GetClassName();
	if (classname == NULL) {
		return false; // Go away
	}

	if (utils::IsVessel(vessel, utils::Saturn)) {
		vessel_type = 1;
	}
	else if (utils::IsVessel(vessel, utils::LEM)) {
		vessel_type = 2;
	}

	switch (messageType) {
	case CDI_GET_VARIABLE:
		sprintf(oapiDebugString(), "REQUEST TO GET VARIABLE, VTYPE %d", vessel_type);
		// Index in val1
		index = m.val1.iValue;
		// Pointer in val2
		value = (char *)m.val2.pValue;
		// Make it happen
		value[0] = 0; // Default
		return true;
		if (vessel_type == 1) {
			// CSM
			Saturn * csm = (Saturn *)vessel;
			strncpy(value, csm->Checklist_Variable[index], 32);
		}
		if (vessel_type == 2) {
			// LEM
			LEM * lem = (LEM *)vessel;
			strncpy(value, lem->Checklist_Variable[index], 32);
		}
		// All others disregarded
		return true;
		break;

	case CDI_SET_VARIABLE:
		// Index in val1
		index = m.val1.iValue;
		// Pointer in val2
		value = (char *)m.val2.pValue;
		// Make it happen
		if (vessel_type == 1) {
			// CSM
			Saturn * csm = (Saturn *)vessel;
			strncpy(csm->Checklist_Variable[index], value, 32);			
		}
		if (vessel_type == 2) {
			// LEM
			LEM * lem = (LEM *)vessel;
			strncpy(lem->Checklist_Variable[index], value, 32);			
		}
		// All others disregarded
		return true;
		break;
	}

	return false;
}

// Original stuff
// Todo: Verify
ChecklistController::ChecklistController(SoundLib &sound) : soundLib(sound)
{
	initCalled = false;
	FileName[0] = 0;
	init(false);
}
// Todo: Verify
ChecklistController::~ChecklistController()
{
}
// Todo: Verify
ChecklistItem *ChecklistController::getChecklistItem(int group, int index)
{
	if (group != -1) {
		spawnCheck(group, false);
	}

	if (active.program.group == -1)
		return NULL;

	if (index + active.sequence->index >= active.set.size())
		return NULL;
	if (index + active.sequence->index < 0)
		return NULL;

	return &(active.set[active.sequence->index + index]);
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
	int failure = input->failGroup;

	if (input->group != active.program.group)
		return false;
	if (input->index >= active.set.size())
		return false;
	active.set[input->index].status = FAILED;
	active.set[input->index].setFlashing(&conn, false);
	iterate();

	if (failure != -1)
		return spawnCheck(failure, true);
	return true;
}
// Todo: Verify
bool ChecklistController::completeChecklistItem(ChecklistItem* input)
{
	int callGroup = input->callGroup;

	if (input->group != active.program.group)
		return false;
	if (input->index >= active.set.size())
		return false;

	active.set[input->index].status = COMPLETE;
	active.set[input->index].setFlashing(&conn, false);
	lastItemTime = lastMissionTime;
	iterate();

	if (callGroup != -1) {
		return spawnCheck(callGroup, false);
	}
	return true;
}
// Todo: Verify
bool ChecklistController::gotoChecklistItem(ChecklistItem* input)
{
	if (input->index >= active.set.size())
		return false;

	ChecklistItem* currentItem = getChecklistItem(-1, 0);
	if (!currentItem)
		return false;

	if (currentItem->index > input->index) {
		for (int i = currentItem->index; i > input->index; i--) {
			active.set[i].status = PENDING;
			active.set[i].setFlashing(&conn, false);
			active.sequence--;
		}

		autoexecuteSlowDelay = active.sequence->getAutoexecuteSlowDelay(&conn);
	}
	else {
		for (int i = currentItem->index; i < input->index; i++) {
			active.set[i].status = COMPLETE;
			active.set[i].setFlashing(&conn, false);
			iterate();
			// We don't want to perform a spawn check; that will perform call groups.
		}
	}

	lastItemTime = lastMissionTime;
	return true;
}
bool ChecklistController::undoChecklistItem() {
	ChecklistItem* lastItem = getChecklistItem(-1, -1);

	// No last item, i.e. this checklist has just begun.
	if (!lastItem) {
		// Special case: this is the "root" checklist. Here be dragons, leave now!
		if (action.empty()) {
			active = ChecklistContainer();
			return true;
		}
		// Exit it and "pop" it from the stack.
		active = action[0];
		action.pop_front();
		
		// Make sure we return to where we were last
		if (active.sequence->index > 0) {
			active.sequence--;
			// Unmark the current item (that called the group we were just in) as completed
			ChecklistItem* curItem = getChecklistItem(-1, 0);
			// N.B. We (probably) don't need to guard on curItem here as,
			// excepting odd race conditions, we should have a current item.
			active.set[curItem->index].status = PENDING;
		}
	}
	else {
		// Unmark the last item as completed and decrement iterator.
		active.set[lastItem->index].status = PENDING;
		active.set[lastItem->index].setFlashing(&conn, true);
		// N.B. we have a last item, so no index check is necessary
		active.sequence--;
	}

	lastItemTime = lastMissionTime;
	autoexecuteSlowDelay = active.sequence->getAutoexecuteSlowDelay(&conn);
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
			// Ignore empty texts
			if (sheet->Cell(i,0)->GetString() != 0) {
				for (int ii = 0; ii < 10 /* Number of columns in accepted sheet */; ii++)
					cells.push_back(*sheet->Cell(i,ii));
				temp.init(cells);
				temp.group = groups.size();
				groups.push_back(temp);
				temp = ChecklistGroup();
				cells = vector<BasicExcelCell>();
			}
		}
	}
	return true;
}

bool ChecklistController::init(char *checkFile, bool SetFileName)
{
	if (SetFileName)
		strncpy(FileName, checkFile, 100);
	return init(checkFile);
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
	oapiWriteScenario_float(scn, "AUTOEXECUTESLOWDELAY", autoexecuteSlowDelay);
	oapiWriteScenario_int(scn, "WAITFORCOMPLETION", (waitForCompletion ? 1 : 0));  

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
		if (!found && !strnicmp(line, "AUTOEXECUTESLOWDELAY", 20))
		{
			float flt = 0;
            sscanf (line + 20, "%f", &flt);
			autoexecuteSlowDelay = flt;
			found = true;
		}
		if (!found && !strnicmp(line, "WAITFORCOMPLETION", 17))
		{
			int i;
            sscanf (line + 17, "%d", &i);
			waitForCompletion = (i != 0);
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
				if (i < groups.size())
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
	cdi.ConnectToVessel(vessel);
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
    autoexecuteSlowDelay = 4;
	autoexecuteAllItemsAutomatic = false;
	playSound = false;
	waitForCompletion = false;

	return true;
}

bool ChecklistController::spawnCheck(int group, bool failed, bool automagic)
{
	if (doSpawnCheck(group, failed, automagic)) {
		waitForCompletion = (active.program.group != -1 && !active.sequence->checkIterate(&conn));
		autoexecuteSlowDelay = active.sequence->getAutoexecuteSlowDelay(&conn);
		return true;
	}
	return false;
}

bool ChecklistController::doSpawnCheck(int group, bool failed, bool automagic)
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
			active.sequence->setFlashing(&conn, false);
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
				active.sequence->setFlashing(&conn, false);
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
			active.sequence->setFlashing(&conn, false);
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
		else {
			active.sequence ++;
			autoexecuteSlowDelay = active.sequence->getAutoexecuteSlowDelay(&conn);
		}
		// Check for vars here?
		if (active.sequence->varlist[0] != 0 && active.sequence->varlist[0] == '$') {
			char * token = NULL;
			// sprintf(oapiDebugString(), "CHECKLIST-ITEM-VARS %s", active.sequence->varlist);
			// Find the first token. It should be delimited by a space or an =
			token = strtok(active.sequence->varlist, "=");
			while (token != NULL) {
				int index = 0;
				char value[32];
				value[0] = 0;
				// Obtain index
				index = atoi(token + 1);
				// sprintf(oapiDebugString(), "INDEX: %d", index);					
				// Obtain string
				token = strtok(NULL, "\""); // Find value
				// sprintf(oapiDebugString(), "TOKEN: %s", token);
				if (strlen(token) > 0) {
					strncpy(value, token, 32);
				} else {
					value[0] = 0;
				}
				// sprintf(oapiDebugString(), "VALUE: %s", value);
				// Set it
				cdi.setVariable(index, value);
				// Obtain next index or EOL
				token = strtok(NULL, ", \n=");
			}
		}
		waitForCompletion = (active.program.group != -1 && !active.sequence->checkIterate(&conn));
	}
}

bool ChecklistController::iterateChecklistItem(double missiontime, SaturnEvents eventController, bool autoexec) {

	if (active.program.group != -1 && active.sequence->checkExec(missiontime, active.startTime, lastItemTime, eventController, (complete && autoexecute && autoexecuteAllItemsAutomatic))) {
		if (active.sequence->iterate(&conn, autoexec)) {
			completeChecklistItem(&(*active.sequence));
			return true;
		} else {
			autoexecuteSlowDelay = active.sequence->getAutoexecuteSlowDelay(&conn);
		}
	}
	return false;
}

bool ChecklistController::isDSKYChecklistItem() {
	
	if (active.program.group != -1) {
		if (!stricmp(active.sequence->item, "DSKY")) {
			return true;
		}
	}
	return false;
}

bool ChecklistController::isDEDAChecklistItem() {

	if (active.program.group != -1) {
		if (!stricmp(active.sequence->item, "DEDA")) {
			return true;
		}
	}
	return false;
}

void ChecklistController::timestep(double missiontime, SaturnEvents eventController)
{
	// Play Sound
	if (playSound) {
		if (checkSound.play())
			playSound = false;
	}

	// Flashing
	if (active.program.group != -1 && active.sequence->checkExec(missiontime + 1, active.startTime, lastItemTime, eventController, true)) {
		active.sequence->setFlashing(&conn, flashing);
	}

	// Even on "non executing" timesteps, we want to allow to complete at least one checklist item
	if (complete && ((autoexecute && !autoexecuteSlow) || (!autoexecute && waitForCompletion))) {
		iterateChecklistItem(missiontime, eventController);
	}

	//Exit if less than one second
	if (missiontime < (lastMissionTime + 1.0))
		return;
	if (complete && ((autoexecute && autoexecuteSlow) || (!autoexecute && !waitForCompletion)) && missiontime < (lastMissionTime + autoexecuteSlowDelay))
		return;
	lastMissionTime = missiontime;

	//Do checklist items
	if (complete) {
		if (autoexecute) {
			if (autoexecuteSlow) {
				iterateChecklistItem(missiontime, eventController, true);
			} else {
				while (iterateChecklistItem(missiontime, eventController, true)) {
					if (isDSKYChecklistItem()) break;
					if (isDEDAChecklistItem()) break;
				}
			}
		} else {
			iterateChecklistItem(missiontime, eventController);
		}
	}

	//Check for groups needing spawn
	for (int i = 0; i < groups.size(); i++)	{
		if (groups[i].autoSelect && !groups[i].called) {
			if(groups[i].checkExec(lastMissionTime,eventController)) {
				spawnCheck(i, false, true);
			}
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

#pragma warning ( pop )
