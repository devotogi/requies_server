#pragma once
class ThreadManager
{
private:
	static ThreadManager*	_instance;
	uint32					_threadId = 0;
	std::vector<HANDLE>		_threads;
	CRITICAL_SECTION		_cs;

public:
	ThreadManager()
	{
		InitializeCriticalSection(&_cs);
	}

	~ThreadManager()
	{
		DeleteCriticalSection(&_cs);
	}

	void Launch(unsigned int (*callback)(void*), void* args);

	static ThreadManager* GetInstance()
	{
		static ThreadManager instance;
		return &instance;
	}
};