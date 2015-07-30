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
