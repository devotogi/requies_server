#pragma once
class Session;

class IOCPCore
{
private:
	HANDLE	_iocpHandle;

public:
	IOCPCore();
	~IOCPCore();

	void RegisterIOCP(Session* session);
	void Dispatch();
};