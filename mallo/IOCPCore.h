#pragma once
class Session;
#include "CThread.h"
class IOCPCore : public CThread
{
private:
	HANDLE	_iocpHandle;

public:
	IOCPCore();
	~IOCPCore();

	void RegisterIOCP(Session* session);
	void Dispatch();

	virtual void Run() override;
};