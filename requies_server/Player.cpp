#include "pch.h"
#include "Player.h"
#include "GameSession.h"
#include "BufferWriter.h"
#include "Map.h"

Player::Player(GameSession* session, int32 sessionId, const Vector3& pos) : _sessionId(sessionId), _pos(pos), _state(State::IDLE), _dir(Dir::NONE), _mouseDir(Dir::NONE), _cameraLocalRotation({ 0,0,0,1 }), _session(session), _prevPos(pos)
{
	InitializeCriticalSection(&_cs);

}

Player::~Player()
{
	EnterCriticalSection(&_cs);
	Map::GetInstance()->Reset(_session);
	LeaveCriticalSection(&_cs);
	DeleteCriticalSection(&_cs);
}

void Player::PlayerSync(const Vector3& pos, State state, Dir dir, Dir mousedir, const Quaternion& cameraLocalRotation)
{
	EnterCriticalSection(&_cs);
	_pos = pos;
	_state = state;
	_dir = dir;
	_cameraLocalRotation = cameraLocalRotation;
	_mouseDir = mousedir;
	LeaveCriticalSection(&_cs);
}