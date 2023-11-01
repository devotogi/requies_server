#include "pch.h"
#include "Monster.h"
#include "BufferWriter.h"
#include "Map.h"
Monster::Monster(int32 monsterId, MonsterType type) : _monsterId(monsterId), _type(type), _state(STATE_NONE)
{
}

Monster::~Monster()
{
}

bool Monster::Attacked(int32 damage)
{
	_hp -= damage;
	_state = ATTACKED;

	if (_hp <= 0) 
	{
		_hp = 0;
		return true;
	}

	return false;
}

void Monster::Update()
{
	State prevState = _state;
	switch (_state)
	{
	case STATE_NONE:
		_state = State::IDLE;
		break;
	case IDLE:
		Update_Idel();
		break;
	case MOVE:
		Update_Move();
		break;
	case ATTACK:
		Update_Attack();
		break;
	case ATTACKED:
		Update_Attacked();
		break;
	case DEATH:
		break;
	}

	if (prevState != _state)
		SyncMonsterPacket();
}

void Monster::Update_Idel()
{
}

void Monster::Update_Move()
{
}

void Monster::Update_Attack()
{
}

void Monster::Update_Attacked()
{
	int32 currentTick = ::GetTickCount64();

	_attackedFps.lastTick = (_attackedFps.lastTick == 0 ? currentTick : _attackedFps.lastTick);

	int32 deltaTick = currentTick - _attackedFps.lastTick;

	_attackedFps.sumTick += deltaTick;
	 
	_attackedFps.lastTick = currentTick;

	if (_attackedFps.sumTick < ATTACKED_TICK)
		return;

	_state = State::IDLE;
	_attackedFps.lastTick = 0;
	_attackedFps.sumTick = 0;
}

void Monster::SyncMonsterPacket()
{
	BYTE sendBuffer[1024] = {};
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();
		
	State state = _state;
	int32 monsterId = _monsterId;
	Vector3 pos = _pos;
	float hp = _hp;
	
	bw.Write(state);
	bw.Write(monsterId);
	bw.Write(pos);
	bw.Write(hp);

	pktHeader->_type = PacketProtocol::S2C_MONSTERSYNC;
	pktHeader->_pktSize = bw.GetWriterSize();

	Map::GetInstance()->BroadCast(_pos,sendBuffer,pktHeader->_pktSize);
}
