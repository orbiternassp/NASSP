/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2023

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
#pragma once
#include <cstdint>

#ifdef _WIN32
#include <winsock.h>
using NativeSocket = SOCKET;
#endif

// Initialize the network subsystem
bool NetStartup();

// Close the network subsystem
void NetCleanup();

class TcpService;

// Move only class that encapsulates a non blocking TCP socket used for reading/writing
// Can be created directly by specifying a destination and port to connect to
// or indirectly by a TcpService instance
class TcpConnection {
	friend TcpService;
public:
	typedef enum {
		UNINITIALIZED,
		CONNECTED
	} ConnectionStatus;

	typedef enum {
		OK,
		RETRY,
		CONNECTION_ERROR,
		UNEXPECTED_ERROR
	} CommandStatus;

	// Create a default UNINITIALIZED TcpConnection
	TcpConnection();

	// Tries to connect to the specified address/port and updates m_status and m_error accordingly
	TcpConnection(const char *dst, uint16_t tcpport);

	// Move only semantics
	TcpConnection(const TcpConnection&) = delete;
	TcpConnection& operator=(TcpConnection const&) = delete;
	TcpConnection(TcpConnection&& from) noexcept;
	TcpConnection& operator=(TcpConnection&& from) noexcept;
	
	// Close() is called upon destruction
	~TcpConnection();
	ConnectionStatus Status() noexcept { return m_status; }

	// Tries to send len bytes to the other peer
	// Return value:
	//  - OK: the data was sent successfully
	//  - CONNECTION_ERROR: the connection was closed
	//  - UNEXPECTED_ERROR: an unexpected error occured
	//  - RETRY : no data could be sent without blocking
	// m_status and m_error are updated accordingly
	CommandStatus Send(const char* buf, size_t len) noexcept;

	// Tries to read one byte from the other peer
	// Return value:
	//  - OK: the data was read successfully
	//  - CONNECTION_ERROR: the connection was closed
	//  - UNEXPECTED_ERROR: an unexpected error occured
	//  - RETRY : no data was available
	// m_status and m_error are updated accordingly
	CommandStatus RecvChar(char* buf) noexcept;

	int ErrorCode() noexcept { return m_error; }

	// Close the connection and sets its status to UNINITIALIZED
	void Close() noexcept;

private:
	// Used by TcpService
	TcpConnection(NativeSocket s);

	ConnectionStatus m_status;
	NativeSocket m_socket;
	int m_error;
};

// Move only class that encapsulates a TCP "server" socket
// Only one client is accepted in the socket queue
class TcpService {
public:
	typedef enum {
		DISABLED,
		STARTED
	} ServiceStatus;

	// Create a default DISABLED TcpService
	TcpService();

	// Create a new service listening on all interfaces on tcpport
	// Sets m_status and m_error accordingly
	TcpService(uint16_t tcpport);

	// Move only semantics
	TcpService(const TcpService&) = delete;
	TcpService& operator=(TcpService const&) = delete;
	TcpService(TcpService&& from) noexcept;
	TcpService& operator=(TcpService&& from) noexcept;

	// The socket is closed on destruction
	~TcpService();

	// Accepts a new connection if a client is trying to connect to the service (non blocking)
	// Returns:
	//  - a CONNECTED TcpConnection if a client connected to the service
	//  - an UNINITIALIZED TcpConnection if there was no client
	TcpConnection Accept() noexcept;

	ServiceStatus Status() noexcept { return m_status; };
	int ErrorCode() noexcept { return m_error; }

private:
	ServiceStatus m_status;
	NativeSocket m_socket;
	int m_error;
};
