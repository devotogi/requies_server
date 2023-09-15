#include "pch.h"
#include "IOCPCore.h"
#include "Session.h"

IOCPCore::IOCPCore()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    int32 threadCount = sysInfo.dwNumberOfProcessors * 2;

    _iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, threadCount);
}

IOCPCore::~IOCPCore()
{
    CloseHandle(_iocpHandle);
}

void IOCPCore::RegisterIOCP(Session* session)
{
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(session->GetSocket()), _iocpHandle, reinterpret_cast<ULONG_PTR>(session), 0);
    session->Recv(0);
}

void IOCPCore::Dispatch()
{
    DWORD numOfBytes = 0;
    LPOVERLAPPED overlapped;
    Session* session = nullptr;

    bool ret = GetQueuedCompletionStatus(_iocpHandle, &numOfBytes, reinterpret_cast<PULONG_PTR>(&session), &overlapped, INFINITE);
    int errorCOde = WSAGetLastError();
    if (ret == false || numOfBytes == 0)
    {
        closesocket(session->GetSocket());
        session->OnDisconnect();
        delete session;
    }
    else
    {
        session->Recv(numOfBytes);
    }
}