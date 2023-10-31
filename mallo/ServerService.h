#pragma once
#include "Service.h"
class TCPListener;
class Session;

class ServerService : public Service
{
protected:
	TCPListener* _tcpListener;

public:
	ServerService(const char* ip, uint16 port, Session* (*sessionFactory)(const SOCKET&, const SOCKADDR_IN&));
	virtual ~ServerService();
	void IOCPRun();
	virtual void Start() override;
	virtual void Run() override;
};

