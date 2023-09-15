#pragma once
class GameSession;
class SessionManager
{
private:
	static SessionManager* _instance;
	std::set<GameSession*> _sessions;
	CRITICAL_SECTION	   _cs;
	uint32				   _sessionId = 0;

public:
	SessionManager()
	{
		InitializeCriticalSection(&_cs);
	}

	~SessionManager()
	{
		DeleteCriticalSection(&_cs);
	}

public:
	void AddSession(GameSession* session);
	void PopSession(GameSession* session);
	void BroadCast(BYTE* dataPtr, int32 dataSize);
	void GetSessionId(int32& sessionId);

	static SessionManager* GetInstance()
	{
		if (_instance == nullptr)
			_instance = new SessionManager();

		return _instance;
	}
};

