#include "pch.h"
#include "TCPConnector.h"

TCPConnector::TCPConnector(const char* connectIp, uint16 connectPort, Session* (*connectServerSession)(const SOCKET&, const SOCKADDR_IN&))
{
    strcpy_s(_connectIp, connectIp);
    _connectPort = connectPort;
    _connectServerSession = connectServerSession;
}

TCPConnector::~TCPConnector()
{

}

Session* TCPConnector::Connect()
{
    SOCKET connectSocket = WSASocketW(PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (connectSocket == INVALID_SOCKET)
        wprintf_s(L"ConnectSocket Is Invalid\n");

    SOCKADDR_IN connectSockAddr;
    memset(&connectSockAddr, 0, sizeof(connectSockAddr));

    connectSockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, _connectIp, &connectSockAddr.sin_addr);
    connectSockAddr.sin_port = htons(_connectPort);

    int32 connectSockAddrSize = sizeof(connectSockAddr);

    if ((::connect(connectSocket, (SOCKADDR*)&connectSockAddr, connectSockAddrSize)) == SOCKET_ERROR)
    {
        wprintf_s(L"Connect Error");
        return nullptr;
    }

    Session* connectSession = _connectServerSession(connectSocket, connectSockAddr);
    return connectSession;
}
