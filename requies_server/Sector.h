#pragma once
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

	std::set<GameSession*>& GetSessions() { return _sessions; }
};

