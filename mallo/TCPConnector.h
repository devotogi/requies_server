#pragma once
class Session;

class TCPConnector
{
private:
	Session* (*_connectServerSession)(const SOCKET&, const SOCKADDR_IN&);
	char	  _connectIp[256];
	uint16	  _connectPort;


public:
	TCPConnector(const char* connectIp, uint16 connectPort, Session* (*connectServerSession)(const SOCKET&, const SOCKADDR_IN&));
	~TCPConnector();

	Session* Connect();
};