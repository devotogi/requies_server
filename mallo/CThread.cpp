#include "pch.h"
#include "CThread.h"

unsigned int CThread::_threadId = 0;

unsigned int _stdcall ThreadProc(void* Args) 
{
	CThread* thread = reinterpret_cast<CThread*>(Args);
	thread->Run();

	return 0;
}

void CThread::ThreadStart()
{
	HANDLE handle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, ThreadProc, this, 0, &_threadId));
	_threadId++;
	_handles.push_back(handle);
}

void CThread::Stop()
{
	for (auto handle : _handles) 
		CloseHandle(handle);
}	

void CThread::Join()
{
	for (auto handle : _handles)
		WaitForSingleObject(handle, INFINITE);
}
