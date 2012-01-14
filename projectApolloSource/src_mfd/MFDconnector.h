/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: MFD Connector header file

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
  *	Revision 1.3  2009/12/17 17:47:18  tschachim
  *	New default checklist for ChecklistMFD together with a lot of related bugfixes and small enhancements.
  *	
  *	Revision 1.2  2009/09/17 17:48:41  tschachim
  *	DSKY support and enhancements of ChecklistMFD / ChecklistController
  *	
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.12  2008/05/24 17:30:40  tschachim
  *	Bugfixes, new flash toggle.
  *	
  *	Revision 1.11  2008/01/25 20:06:06  lassombra
  *	Implemented delayable switch functions.
  *	
  *	Now, all register functions on all toggle switches should take, at the end, a boolean
  *	 for whether it is delayable, and an int for how many seconds to delay.
  *	
  *	Actual delay can be anywhere between the int and the int + 1.
  *	
  *	Function is implemented as a timestepped switch which is called intelligently from
  *	 the panel, which now gets a timestep call.
  *	
  *	Revision 1.10  2008/01/23 01:40:07  lassombra
  *	Implemented timestep functions and event management
  *	
  *	Events for Saturns are now fully implemented
  *	
  *	Removed all hardcoded checklists from Saturns.
  *	
  *	Automatic Checklists are coded into an excel file.
  *	
  *	Added function to get the name of the active checklist.
  *	
  *	ChecklistController is now 100% ready for Saturn.
  *	
  *	Revision 1.9  2008/01/16 19:03:02  lassombra
  *	All but time-step, automation, and complete save/load is now implemented on the Checklist Controller (and the files that depend on it).
  *	
  *	All bugs in current code should hopefully be gone, but not necessarily so.
  *	
  *	Revision 1.8  2008/01/15 19:13:05  lassombra
  *	Implemented Vessel connector and Checklist Completion step.  The third button down on the left side completes the "current" step on the temporary MFD.
  *	
  *	Additionally made dummy checklists reference "Mission Timer Switch" so that the flashing function can be tested.
  *	
  *	Finally, updated the MFD demo to show one way to do a flashing switch, and a recommended deconstructor implementation.
  *	
  *	Revision 1.7  2008/01/15 17:43:59  lassombra
  *	Allows multiple senders to attach to a single receiver.
  *	
  *	Revision 1.6  2008/01/14 15:52:34  lassombra
  *	*Final* version of the interface for the checklist controller.  May need some more
  *	 data, but should be accessible at this point.  For some reason getting heap errors
  *	 in deconstructors, doing research into it.
  *	
  *	Revision 1.5  2008/01/09 09:39:06  lassombra
  *	Completed MFD<->ChecklistController interface.  Coding can now take place on two separate code paths.
  *	
  *	Anyone who wants to work on the MFD can at this point do so using the existing connector code.
  *	
  *	None of the functions will exactly DO anything at the moment, but that is being worked on.
  *	
  *	Revision 1.4  2008/01/09 01:46:45  movieman523
  *	Added initial support for talking to checklist controller from MFD.
  *	
  *	Revision 1.3  2007/12/21 02:47:08  movieman523
  *	Connector cleanup, and fix my build break!
  *	
  *	Revision 1.2  2007/12/21 02:31:17  movieman523
  *	Added SetState() call and some more documentation.
  *	
  *	Revision 1.1  2007/12/21 01:00:30  movieman523
  *	Really basic Checklist MFD based on Project Apollo MFD, along with the various support functions required to make it work.
  *	
  **************************************************************************/

#if !defined(_PA_MFDCONNECTOR_H)
#define _PA_MFDCONNECTOR_H

///
/// \ingroup Connectors
/// \brief MFD to panel connector class.
///
class MFDConnector : public Connector
{
public:
	MFDConnector();
	~MFDConnector();

	///
	/// Connect to a vessel. If already connected, this call will
	/// disconnect first.
	///
	/// \param v Vessel to attempt to connect to.
	/// \return True if we succeeded.
	///
	bool ConnectToVessel(VESSEL *v);
	
	///
	/// Do the actual connection.
	///
	/// \Param other, connector to connect to.
	/// \Return true if succeded.
	///
	bool ConnectTo(Connector *other);

	///
	/// Disconnect from the current vessel, if connected.
	///
	void DisconnectVessel();

	///
	/// Set a panel item flashing, or stop it flashing.
	///
	/// \param n Name of panel item.
	/// \param flash True to start flashing, false to stop flashing.
	/// \return True if the call succeeded.
	///
	bool SetFlashing(char *n, bool flash);

	///
	/// Get the state of a panel item.
	///
	/// \param n Name of panel item.
	/// \return Integer value if item found, -1 if not.
	///
	int GetState(char *n);

	///
	/// Set the state of a panel item.
	///
	/// \param n Name of panel item.
	/// \param value Value to set state to.
	/// \return True if call succeeded, false if failed.
	///
	bool SetState(char *n, int value, bool guard = false, bool hold = false);

	///
	/// Get the failure state of a panel item.
	///
	/// \param n Name of panel item.
	/// \return True if failed, false if not (or if the connector isn't connected).
	///
	bool GetFailed(char *n);

	///
	/// Tell the checklist to Auto-complete checklist items each timestep.
	///
	/// \param yesno setting of auto-complete
	/// \return True if successful, false if failed.
	///
	bool ChecklistAutocomplete(bool yesno);

	///
	/// Get a checklist item with an initialized checklist item as the source pointer.
	///
	/// \param in checklistItem initialized to tell the controller which item to return.
	/// \return ChecklistItem returned by controller or false if connector isn't connected.
	///

	ChecklistItem *GetChecklistItem(int group, int index);

	///
	/// Get a list of all allowed checklist items.
	///
	/// \return pointer to the first element in an array that is null terminated. (group index of last element is 0)
	///
	vector<ChecklistGroup> *GetChecklistList();

	///
	/// Fail a checklist item, and branch if appropriate.
	///
	/// \param in ChecklistItem that is to be failed.
	/// \return true if item was successfully failed.  False otherwise.
	///
	bool failChecklistItem(ChecklistItem* in);
	
	///
	/// Complete a checklist item.
	///
	/// \param in ChecklistItem that is completed.
	/// \return true if item was successfully "completed".  False otherwise.
	///
	bool completeChecklistItem(ChecklistItem* in);

	///
	/// Get the autoComplete status.
	///
	/// \return autoComplete status.
	///
	bool ChecklistAutocomplete();

	///
	/// Get the title of the current checklist
	///
	/// \return char *
	///
	char *checklistName();

	///
	/// Get an entire checklist
	///
	/// \param ChecklistContainer with program initialized to group desired
	///
	bool RetrieveChecklist(ChecklistContainer *);

	bool GetChecklistFlashing();

	void SetChecklistFlashing(bool f);

	bool GetFlashing(char *n);

protected:
};

#endif // _PA_MFDCONNECTOR_H
