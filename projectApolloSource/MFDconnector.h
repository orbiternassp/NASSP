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
	bool SetState(char *n, int value);

protected:
};

#endif // _PA_MFDCONNECTOR_H
