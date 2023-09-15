#include "pch.h"
#include "ClientService.h"
#include "TCPConnector.h"
#include "IOCPCore.h"
ClientService::ClientService(int32 maxSession, const char* connetIp, uint16 connetPort, Session* (*clientSessionFactory)(const SOCKET&, const SOCKADDR_IN&)) : Service(connetIp, connetPort, clientSessionFactory), _maxSession(maxSession)
{
    _tcpConnector = new TCPConnector(connetIp, connetPort, clientSessionFactory);
}

ClientService::~ClientService()
{
    if (_tcpConnector)
        delete _tcpConnector;
}

void ClientService::Start()
{
    wprintf(L"ClientServerStart\n");
    while (true)
    {
        try
        {
            for (int i = 0; i < _maxSession; i++)
            {
                Session* newSession = _tcpConnector->Connect();
                _iocpCore->RegisterIOCP(newSession);
            }
        }
        catch (std::wstring error)
        {
            wprintf(L"ClientServerStart Error\n");
        }
    }
}
