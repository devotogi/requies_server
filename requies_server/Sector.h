#pragma once
#include "Monster.h"
class GameSession;
class Sector
{
private:
	CRITICAL_SECTION		_cs;
	std::set<GameSession*> _sessions;
public:
	Sector();
	~Sector();

	void Set(GameSession* session);
	void Reset(GameSession* session);
	void BroadCast(GameSession* session, BYTE* sendBuffer, int32 sendSize);
	void SendPlayerList(GameSession* session);
	void SendRemoveList(GameSession* session);
	void SendRemoveMonster(Monster* monster, int32 sendSize);
	void SendNewMonster(Monster* monster, int32 sendSize);
	std::set<GameSession*>& GetSessions() { return _sessions; }
};

