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

#include "socket.h"
#include <assert.h>

bool NetStartup() {
#ifdef _WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	return (iResult == NO_ERROR);
#endif
}
void NetCleanup() {
#ifdef _WIN32
	WSACleanup();
#endif
}

TcpConnection::TcpConnection() {
	m_error = 0;
	m_status = ConnectionStatus::UNINITIALIZED;
	m_socket = INVALID_SOCKET;
}

TcpConnection::TcpConnection(const char *dst, uint16_t tcpport) {
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET) {
		m_status = ConnectionStatus::UNINITIALIZED;
		m_error = WSAGetLastError();
		return;
	}

	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(dst);
	clientService.sin_port = htons(tcpport);

	if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		m_status = ConnectionStatus::UNINITIALIZED;
		m_error = WSAGetLastError();
		Close();
		return;
	}
	m_status = ConnectionStatus::CONNECTED;
}

TcpConnection::TcpConnection(TcpConnection&& from) noexcept {
	m_error = from.m_error;
	m_status = from.m_status;
	m_socket = from.m_socket;
	from.m_socket = INVALID_SOCKET;
	from.m_status = ConnectionStatus::UNINITIALIZED;
}

TcpConnection& TcpConnection::operator=(TcpConnection&& from) noexcept {
	Close();
	m_error = from.m_error;
	m_status = from.m_status;
	m_socket = from.m_socket;
	from.m_socket = INVALID_SOCKET;
	from.m_status = ConnectionStatus::UNINITIALIZED;
	return *this;
}

TcpConnection::~TcpConnection() {
	Close();
}

TcpConnection::CommandStatus TcpConnection::Send(const char* buf, size_t len) noexcept {
	m_error = 0;
	int bytesSent = send(m_socket, buf, len, 0);
	if (bytesSent == SOCKET_ERROR) {
		m_error = WSAGetLastError();
		switch (m_error) {
			// KNOWN CODES that we can ignore
		case 10035: // Operation Would Block
			// We can ignore this entirely. It's not an error.
			return CommandStatus::RETRY;

		case 10038: // Socket isn't a socket
		case 10053: // Software caused connection abort
		case 10054: // Connection reset by peer
			Close();
			return CommandStatus::CONNECTION_ERROR;

		default:    // If unknown
			Close();
			return CommandStatus::UNEXPECTED_ERROR;
			break;
		}
	}
	return OK;
}

TcpConnection::CommandStatus TcpConnection::RecvChar(char* buf) noexcept {
	int bytesRecv = recv(m_socket, buf, 1, 0);
	assert(bytesRecv != 0);
	if (bytesRecv == SOCKET_ERROR) {
		m_error = WSAGetLastError();
		switch (m_error) {
			// KNOWN CODES that we can ignore
		case 10035: // Operation Would Block
			// We can ignore this entirely. It's not an error.
			m_error = 0;
			return RETRY;

		case 10053: // Software caused connection abort
		case 10038: // Socket isn't a socket
		case 10054: // Connection reset by peer
			Close();
			return CommandStatus::CONNECTION_ERROR;

		default:    // If unknown
			Close();
			return CommandStatus::UNEXPECTED_ERROR;
		}
	}
	m_error = 0;
	return OK;
}

void TcpConnection::Close() noexcept {
	if (m_socket != INVALID_SOCKET) {
		shutdown(m_socket, 2); // Shutdown both streams
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	m_status = ConnectionStatus::UNINITIALIZED;
}

TcpConnection::TcpConnection(NativeSocket s) {
	m_error = 0;
	m_socket = s;
	m_status = ConnectionStatus::CONNECTED;
}

TcpService::TcpService() {
	m_status = ServiceStatus::DISABLED;
}

TcpService::TcpService(TcpService&& from) noexcept {
	m_status = from.m_status;
	m_socket = from.m_socket;
	m_error = from.m_error;
	from.m_status = TcpService::DISABLED;
	from.m_socket = INVALID_SOCKET;
}

TcpService& TcpService::operator=(TcpService&& from) noexcept {
	m_status = from.m_status;
	m_socket = from.m_socket;
	m_error = from.m_error;
	from.m_status = TcpService::DISABLED;
	from.m_socket = INVALID_SOCKET;
	return *this;
}

TcpService::~TcpService() {
	if (m_socket != INVALID_SOCKET) {
		closesocket(m_socket);
	}
}

TcpService::TcpService(uint16_t tcpport) {
	m_status = ServiceStatus::DISABLED;

	sockaddr_in service;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET) {
		m_error = WSAGetLastError();
		NetCleanup();
		return;
	}
	// Be nonblocking
	int iMode = 1; // 0 = BLOCKING, 1 = NONBLOCKING
	if (ioctlsocket(m_socket, FIONBIO, (u_long FAR*) & iMode) != 0) {
		m_error = WSAGetLastError();
		closesocket(m_socket);
		NetCleanup();
		return;
	}

	// Set up incoming options
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons(tcpport);

	if (::bind(m_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		m_error = WSAGetLastError();
		closesocket(m_socket);
		NetCleanup();
		return;
	}
	if (listen(m_socket, 1) == SOCKET_ERROR) {
		m_error = WSAGetLastError();
		closesocket(m_socket);
		NetCleanup();
		return;
	}

	m_status = ServiceStatus::STARTED;
	return;
}

TcpConnection TcpService::Accept() noexcept {
	NativeSocket s = accept(m_socket, NULL, NULL);
	if (s == INVALID_SOCKET) {
		m_error = WSAGetLastError();
		return TcpConnection();
	}
	else {
		m_error = 0;
		return TcpConnection(s);
	}
}
