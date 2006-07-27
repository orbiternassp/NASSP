/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Connector class

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
  *	Revision 1.5  2006/07/27 20:40:06  movieman523
  *	We can now draw power from the SIVb in the Apollo to Venus scenario.
  *	
  *	Revision 1.4  2006/07/21 23:04:34  movieman523
  *	Added Saturn 1b engine lights on panel and beginnings of electrical connector work (couldn't disentangle the changes). Be sure to get the config file for the SIVb as well.
  *	
  *	Revision 1.3  2006/07/09 16:09:38  movieman523
  *	Added Prog 59 for SIVb venting.
  *	
  *	Revision 1.2  2006/07/09 00:07:07  movieman523
  *	Initial tidy-up of connector code.
  *	
  *	Revision 1.1  2006/07/07 19:35:24  movieman523
  *	First version.
  *	
  **************************************************************************/

#if !defined(_PA_CONNECTOR_H)
#define _PA_CONNECTOR_H

///
/// \ingroup Connectors
/// \brief Connector type.
///
enum ConnectorType
{
	NO_CONNECTION,				///< Dummy.
	CSM_IU_COMMAND,				///< Passes commands and data between CSM and IU.
	LV_IU_COMMAND,				///< Passes commands between launch vehicle and IU.
	CSM_SIVB_DOCKING,			///< Docking connector between CSM and SIVb.
	CSM_LEM_DOCKING,			///< Docking connector between CSM and LEM.
	CSM_SIVB_COMMAND,			///< Passes commands and data between CSM and SIVb.
	CSM_SIVB_POWER,				///< Power connection from SIVB to CSM.
	LEM_CSM_POWER,				///< Power connection from CSM to LEM.
};

//
// The message can pass various different parameters in this union. The receiver
// will determine which is correct based on the message type.
//
// \brief Message value for connector messages.
//
union ConnectorMessageValue
{
	int iValue;				///< Integer message value.
	double dValue;			///< Floating point message value.
	bool bValue;			///< Boolean message value.
	void *pValue;			///< Pointer message value.
	OBJHANDLE hValue;		///< Orbiter handle.
};

///
/// \ingroup Connectors
/// \brief Connector message to be passed through the system.
///
struct ConnectorMessage
{
	///
	/// \brief What kind of connector should this message go to?
	///
	ConnectorType destination;

	///
	/// \brief Connection-specific message type.
	///
	unsigned int messageType;

	///
	/// \brief Message value 1.
	///
	ConnectorMessageValue val1;

	///
	/// \brief Message value 2.
	///
	ConnectorMessageValue val2;

	///
	/// \brief Message value 3.
	///
	ConnectorMessageValue val3;
};

///
/// \ingroup Connectors
/// \brief Connector class. Specific connectors will be derived from this class.
///
class Connector {
public:
	///
	/// \brief Constructor.
	///
	Connector();

	///
	/// \brief Destructor. Disconnects the connector when called.
	///
	virtual ~Connector();

	///
	/// \brief Get the type of the connector.
	/// \return Connector type.
	///
	virtual ConnectorType GetType();

	///
	/// \brief Set the type of the connector.
	/// \param t Connector type.
	///
	void SetType(ConnectorType t) { type = t; };

	///
	/// \brief Connect to another connector.
	/// \param other Other end of the connection.
	/// \return True if the connector is the correct type and we connected.
	///
	virtual bool ConnectTo(Connector *other);

	///
	/// \brief Disconnect from the far end of the connection.
	///
	virtual void Disconnect();

	///
	/// \brief Has been disconnected from the far end of the connection.
	///
	virtual void Disconnected();

	///
	/// Send a message through the connector. Note that the receiver can update the value in the
	/// connector message, in order to return data to the caller.
	///
	/// \brief Send message.
	/// \param m Message to send.
	/// \return False if the connector isn't connected to anything or the message wasn't handled
	/// at the far end of the connection.
	///
	virtual bool SendMessage(ConnectorMessage &m);

	///
	/// Receieve a message through the connector. Note that the receiver can update the value in the
	/// connector message, in order to return data to the caller.
	///
	/// The default connector is output-only, and will return an error if the other end of the connection
	/// tries to send it data.
	///
	/// \brief Receive message.
	/// \param from The connector that sent the message.
	/// \param m Message received.
	/// \return False if we don't handle the message.
	///
	virtual bool ReceiveMessage(Connector *from, ConnectorMessage &m);

	///
	/// \brief Connector we're connected to, if any.
	///
	Connector *connectedTo;

protected:
	///
	/// \brief Type of connection.
	///
	ConnectorType type;
};

///
/// \ingroup Connectors
/// \brief Connector class for multiple connectors, typically used for docked ships.
///
class MultiConnector : public Connector
{
public:
	MultiConnector();
	~MultiConnector();

	///
	/// \brief Add another connection to this connector.
	/// \param other Connector to add.
	/// \return True if we could add the new connector.
	///
	virtual bool AddTo(Connector *other);

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
	void Disconnect();

#define N_MULTICONNECT_INPUTS 16

private:
	Connector *Inputs[N_MULTICONNECT_INPUTS];
};

class PowerDrainConnectorObject;

///
/// \ingroup Connectors
/// \brief Connector class for power drain.
///
class PowerDrainConnector : public Connector
{
public:
	PowerDrainConnector();
	~PowerDrainConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
	void Disconnected();

	void SetPowerDrain(PowerDrainConnectorObject *p);

private:
	PowerDrainConnectorObject *power_drain;
};

#endif // _PA_CONNECTOR_H
