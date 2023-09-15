#pragma once
#include "Session.h"
class Player;

class GameSession : public Session
{
private:
	Player* _player;
public:
	static Session* MakeGameSession(const SOCKET& socket, const SOCKADDR_IN& sockAddr) { return new GameSession(socket, sockAddr); }

public:
	Player* GetPlayer() { return _player; }

public:
	GameSession(const SOCKET& socket, const SOCKADDR_IN& sock);
	virtual ~GameSession();

	virtual void OnRecv(Session* session, BYTE* dataPtr, int32 dataLen);
	virtual void OnDisconnect();
	virtual void OnConnect();
};

