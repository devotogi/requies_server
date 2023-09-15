#pragma once
class Session;

class TCPListener
{
private:
	SOCKET		_listenSocket = INVALID_SOCKET;
	SOCKADDR_IN	_serverAddr = { 0 };
	Session*	(*_sessionFactory)(const SOCKET&, const SOCKADDR_IN&);

public:
	TCPListener(const char* ip, uint16 port, Session* (*sessionFactory)(const SOCKET&, const SOCKADDR_IN&));
	~TCPListener();

	void	 Listen();
	Session* Accept();
};

