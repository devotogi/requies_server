#include "pch.h"
#include "Service.h"
#include "TCPListener.h"
#include "IOCPCore.h"

Service::Service(const char* ip, uint16 port, Session* (*sessionFactory)(const SOCKET&, const SOCKADDR_IN&))
{
    WSADATA		wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        wprintf(L"WSAStartUpError");
    }

    _iocpCore = new IOCPCore();
}

Service::~Service()
{
    if (_iocpCore)
        delete _iocpCore;
}