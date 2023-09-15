#include "pch.h"
#include "SessionManager.h"
#include "GameSession.h"
#include "BufferWriter.h"
#include "Player.h"

SessionManager* SessionManager::_instance = nullptr;

void SessionManager::AddSession(GameSession* session)
{
	EnterCriticalSection(&_cs);

	_sessions.insert(session);

	LeaveCriticalSection(&_cs);
}

void SessionManager::PopSession(GameSession* session)
{
	EnterCriticalSection(&_cs);

	_sessions.erase(session);

	LeaveCriticalSection(&_cs);
}

void SessionManager::BroadCast(BYTE* dataPtr, int32 dataSize)
{
	EnterCriticalSection(&_cs);

	for (auto& session : _sessions)
		session->Send(dataPtr, dataSize);

	LeaveCriticalSection(&_cs);
}

void SessionManager::GetSessionId(int32& sessionId)
{
	EnterCriticalSection(&_cs);

	_sessionId++;
	sessionId = _sessionId;

	LeaveCriticalSection(&_cs);
}
