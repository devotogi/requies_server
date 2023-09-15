#pragma once
#include "RecvBuffer.h"

class Session
{
protected:
	WSAOVERLAPPED	_overlapped;
	SOCKET			_socket;
	SOCKADDR_IN		_sockaddrIn;
	RecvBuffer	    _recvBuffer;
	int32			_sessionId;

public:
	Session(const SOCKET& socket, const SOCKADDR_IN& sock);
	virtual ~Session();

	SOCKET& GetSocket() { return _socket; }
	void    Send(BYTE* buffer, int32 bufferSize);
	void	Recv(int32 numOfBytes);
	int32 GetSessionID() { return _sessionId; }

public:
	virtual void	OnRecv(Session* session, BYTE* dataPtr, int32 dataLen);
	virtual void	OnDisconnect();
	virtual void    OnConnect();
};

