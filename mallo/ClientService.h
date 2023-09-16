#pragma once
#include "Service.h"
class TCPConnector;
class ClientService : public Service
{
protected:
	TCPConnector*	_tcpConnector;
	int32			_maxSession;

public:
	ClientService(int32 maxSession, const char* connetIp, uint16 connetPort, Session* (*clientSessionFactory)(const SOCKET&, const SOCKADDR_IN&));
	virtual ~ClientService();

	virtual void Start();

};