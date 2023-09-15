#include "pch.h"
#include "TCPListener.h"
#include "Session.h"

TCPListener::TCPListener(const char* ip, uint16 port, Session* (*sessionFactory)(const SOCKET&, const SOCKADDR_IN&)) : _sessionFactory(sessionFactory)
{
    _listenSocket = WSASocketW(PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (_listenSocket == INVALID_SOCKET)
        wprintf_s(L"ListenSocket Is Invalid\n");

    memset(&_serverAddr, sizeof(_serverAddr), 0);

    _serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &_serverAddr.sin_addr);
    _serverAddr.sin_port = htons(port);

    int32 serverAddrLen = sizeof(_serverAddr);

    if (::bind(_listenSocket, reinterpret_cast<SOCKADDR*>(&_serverAddr), serverAddrLen) == INVALID_SOCKET)
        wprintf_s(L"Socket Bind Error\n");
}

TCPListener::~TCPListener()
{
    ::closesocket(_listenSocket);
    WSACleanup();
}

void TCPListener::Listen()
{
    if (::listen(_listenSocket, 5) == SOCKET_ERROR)
        wprintf_s(L"Listten Error\n");
}

Session* TCPListener::Accept()
{
    SOCKADDR_IN clientAddr;
    int32 clientAddrLen = sizeof(clientAddr);

    SOCKET clientSocket = accept(_listenSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrLen);
    Session* newSession = _sessionFactory(clientSocket, clientAddr);
    newSession->OnConnect();

    return newSession;
}
