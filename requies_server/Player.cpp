#include "pch.h"
#include "Player.h"
#include "GameSession.h"
#include "BufferWriter.h"
#include "Map.h"
#include "MapDataManager.h"

Player::Player(GameSession* session, int32 sessionId, const Vector3& pos) : _sessionId(sessionId), _state(State::IDLE), _dir(Dir::NONE), _mouseDir(Dir::NONE), _cameraLocalRotation({ 0,0,0,1 }), _session(session), _prevPos(pos), GameObject(pos)
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

void Player::PlayerSync(const Vector3& pos, State state, Dir dir, Dir mousedir, const Quaternion& cameraLocalRotation, const Vector3& target, MoveType moveType, Quaternion localRotation)
{
	EnterCriticalSection(&_cs);
	_pos = pos;
	_state = state;
	_dir = dir;
	_cameraLocalRotation = cameraLocalRotation;
	_mouseDir = mousedir;
	_target = target;
	_moveType = moveType;
	_localRotation = localRotation;
	LeaveCriticalSection(&_cs);
}

void Player::Attacked(int damage)
{
	if (_death) return;

	_hp -= damage;

	if (_hp <= 0) _hp = 0;

	// TODO 죽었는지 처리
	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(_sessionId);
	bw.Write(_hp);
	bw.Write(_mp);

	pktHeader->_type = PacketProtocol::S2C_PLAYERATTACKED;
	pktHeader->_pktSize = bw.GetWriterSize();

	Map::GetInstance()->BroadCast(_session, sendBuffer, bw.GetWriterSize());

	if (_hp == 0) 
	{
		_death = true;
		// 플레이어 죽음
		BYTE sendBuffer[100];
		BufferWriter bw(sendBuffer);
		PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

		bw.Write(_sessionId);
		pktHeader->_type = PacketProtocol::S2C_PLAYERDETH;
		pktHeader->_pktSize = bw.GetWriterSize();

		Map::GetInstance()->BroadCast(_session, sendBuffer, bw.GetWriterSize());
	}
}

void Player::ReSpawn()
{
	Map::GetInstance()->Reset(_session);
	MapDataManager::GetInstnace()->ApplyMapGameObject(this, false);

	EnterCriticalSection(&_cs);
	_pos = { 67,0,72 };
	_prevPos = { 67,0,72 };
	_state = IDLE;
	_dir = NONE;
	_mouseDir = NONE;
	LeaveCriticalSection(&_cs);

	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	int32 sessionId = _sessionId;
	uint16 playerState = (uint16)_state;
	uint16 playerDir = (uint16)_dir;
	uint16 playerMouseDir = (uint16)_mouseDir;
	Vector3 playerPos = _pos;
	Quaternion playerQuaternion = _cameraLocalRotation;
	_hp = 1000;
	_mp = 1000;
	float hp = _hp;
	float mp = _mp;

	bw.Write(sessionId);
	bw.Write(playerState);
	bw.Write(playerDir);
	bw.Write(playerMouseDir);
	bw.Write(playerPos);
	bw.Write(playerQuaternion);
	bw.Write(hp);
	bw.Write(mp);
	_death = false;
	pktHeader->_type = PacketProtocol::S2C_PLAYERESPAWN;
	pktHeader->_pktSize = bw.GetWriterSize();

	_session->Send(sendBuffer, pktHeader->_pktSize);
	Map::GetInstance()->Set(_session);
	MapDataManager::GetInstnace()->ApplyMapGameObject(this, true);
}
