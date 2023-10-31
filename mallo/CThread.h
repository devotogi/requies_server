#pragma once
class CThread
{
private:
	static unsigned int _threadId;
	std::vector<HANDLE> _handles;

public:
	virtual void Run() abstract;

	void ThreadStart();
	void Stop();
	void Join();
};