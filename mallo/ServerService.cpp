#include "pch.h"
#include "ServerService.h"
#include "TCPListener.h"
#include "IOCPCore.h"

ServerService::ServerService(const char* ip, uint16 port, Session* (*sessionFactory)(const SOCKET&, const SOCKADDR_IN&)) : Service(ip, port, sessionFactory)
{
    _tcpListener = new TCPListener(ip, port, sessionFactory);
}

ServerService::~ServerService()
{
    if (_tcpListener)
        delete _tcpListener;
}

void ServerService::IOCPRun()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    int32 threadCount = sysInfo.dwNumberOfProcessors * 2;

    for (int i = 0; i < threadCount; i++)
        _iocpCore->ThreadStart();
}

void ServerService::Start()
{
    wprintf(L"ServerStart\n");
    while (true)
    {
        try
        {
            _tcpListener->Listen();
            Session* newSession = _tcpListener->Accept();
            _iocpCore->RegisterIOCP(newSession);
        }
        catch (std::wstring error)
        {
            wprintf(L"ServerStart Error\n");
        }
    }
}

void ServerService::Run()
{
    ServerService::Start();
}
