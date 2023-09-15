#pragma once
class TCPListener;
class IOCPCore;
class Session;

class Service
{
protected:
	IOCPCore* _iocpCore;

public:
	Service(const char* ip, uint16 port, Session* (*sessionFactory)(const SOCKET&, const SOCKADDR_IN&));
	virtual ~Service();

	IOCPCore* GetIOCPCore() { return _iocpCore; }
	virtual void Start() abstract;
};