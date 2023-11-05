#include "pch.h"
#include "Monster.h"
#include "BufferWriter.h"
#include "Map.h"
#include "MapDataManager.h"
#include "Player.h"
#include "Sector.h"
#include "GameSession.h"
#include "SpawnZone.h"
Monster::Monster(const Pos& spawnZoneIndex, int32 monsterId, MonsterType type, const Vector3& pos) : _spawnZoneIndex(spawnZoneIndex), _monsterId(monsterId), _type(type), _state(STATE_NONE), GameObject(pos)
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
	
	case ATTACK:
		Update_Attack();
		break;
	
	case ATTACKED:
		Update_Attacked();
		break;

	case DEATH:
		break;

	case COOL_TIME:
		Update_COOL_TIME();
		break;

	case PATROL:
		Update_Patrol();
		break;

	case TRACE:
		Update_Trace();
		break;
	}

	if (prevState != _state)
		SyncMonsterPacket();
}

void Monster::Dead()
{
	int32 posZ = static_cast<int32>(_pos.z) / 32;
	int32 posX = static_cast<int32>(_pos.x) / 32;

	Map::GetInstance()->SpawnZones()[posZ][posX]->RemoveMonster(_monsterId);
}

void Monster::Update_Idel()
{
	int32 currentTick = ::GetTickCount64();

	_searchFps.lastTick = (_searchFps.lastTick == 0 ? currentTick : _searchFps.lastTick);

	int32 deltaTick = currentTick - _attackedFps.lastTick;

	_searchFps.sumTick += deltaTick;

	_searchFps.lastTick = currentTick;

	if (_searchFps.sumTick < SEARCH_TICK)
		return;

	AttackSearch();

	if (_state == ATTACK)
	{
		_searchFps.sumTick = 0;
		return;
	}

	TraceSearch();

	if (_state == TRACE)
	{
		_searchFps.sumTick = 0;
		return;
	}

 	PatrolSearch();

	_searchFps.sumTick = 0;
}

void Monster::Update_Attack()
{
	std::vector<std::vector<std::set<GameObject*>>>& mapDataGameObejct = MapDataManager::GetInstnace()->MapDataGameObject();

	Vector3 targetPos = _target->GetPos();

	int32 x = static_cast<int32>(targetPos.x);
	int32 z = static_cast<int32>(targetPos.z);

	auto find = mapDataGameObejct[z][x].find(_target);

	if (find != mapDataGameObejct[z][x].end())
	{
		static_cast<Player*>((*find))->Attacked(_damage);
	}

	_state = State::COOL_TIME;
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

void Monster::Update_Trace()
{
	int32 currentTick = ::GetTickCount64();
	const int32 moveTick = MOVE_TICK / _speed;

	int32 deltaTick = currentTick - _moveFps.lastTick;

	_moveFps.sumTick += deltaTick;

	_moveFps.lastTick = currentTick;

	if (_moveFps.sumTick < moveTick)
		return;

	std::vector<Pos> path;
	Vector3 playerPos = _target->GetPos();
	MapDataManager::GetInstnace()->FindPath(playerPos, _pos, path);

	if (path.size() == 0)
	{
		_state = IDLE;
		_moveFps.sumTick = 0;
		_moveFps.lastTick = 0;
		return;
	}

	float nx = path[0].x + 0.5f;
	float nz = path[0].z + 0.5f;

	float moveDist = _speed * (moveTick / 1000.f);
	Vector3 dirVector = Vector3{ nx, 0, nz } - _pos;
	_dir = dirVector.Normalized();
	float dist = abs(_pos.x - playerPos.x) + abs(_pos.z - playerPos.z);

	if (dist >= 20.f)
	{
		_state = IDLE;
		_moveFps.sumTick = 0;
		_moveFps.lastTick = 0;
		return;
	}

	if (path.size() == 1)
	{
		_state = IDLE;
		_moveFps.sumTick = 0;
		_moveFps.lastTick = 0;
		return;
	}
	else
	{
		Vector3 nextPos = _pos + (_dir * moveDist);

		int32 nextX = static_cast<int32>(nextPos.x);
		int32 nextZ = static_cast<int32>(nextPos.z);

		Pos nextPPos = { nextX, nextZ };
		Pos nowPPos = { _pos.x, _pos.z };

		int32 nowX = static_cast<int32>(_pos.x);
		int32 nowZ = static_cast<int32>(_pos.z);

		int32 nextZIndex = nextZ / 32;
		int32 nextXIndex = nextX / 32;

		int32 nowZIndex = nowZ / 32;
		int32 nowXIndex = nowX / 32;

		if (nextXIndex != nowXIndex || nextZIndex != nowZIndex)
		{
			bool moveMonster = Map::GetInstance()->SpawnZones()[nextZIndex][nextXIndex]->Exist(_monsterId);
			if (moveMonster == false) // 몬스터가 Sector를 이동함
			{
				Map::GetInstance()->SpawnZones()[nowZIndex][nowXIndex]->Reset(this, nowPPos);
				Map::GetInstance()->SpawnZones()[nextZIndex][nextXIndex]->Set(this, nowPPos);
			}
		}

		std::cout << _moveFps.sumTick << std::endl;
		SyncMonsterPacket();
		_pos = nextPos;
		_moveFps.sumTick = 0;
	}
}

void Monster::Update_Patrol()
{
	int32 currentTick = ::GetTickCount64();
	const int32 moveTick = MOVE_TICK / _speed;

	int32 deltaTick = currentTick - _moveFps.lastTick;

	_moveFps.sumTick += deltaTick;

	_moveFps.lastTick = currentTick;

	if (_moveFps.sumTick < moveTick)
		return;

	std::vector<Pos> path;
	Vector3 target = _patrolTarget;
	MapDataManager::GetInstnace()->FindPath(target, _pos, path);

	float dist = abs(_pos.x - target.x) + abs(_pos.z - target.z);

	if (dist <= 0.5f)
	{
		_state = IDLE;
		_moveFps.sumTick = 0;
		_moveFps.lastTick = 0;
		return;
	}

	if (path.size() == 0)
	{
		_state = IDLE;
		_moveFps.sumTick = 0;
		_moveFps.lastTick = 0;
		return;
	}

	float nx = path[0].x + 0.5f;
	float nz = path[0].z + 0.5f;

	float moveDist = _speed * (moveTick / 1000.f);
	Vector3 dirVector = Vector3{ nx, 0, nz } - _pos;
	_dir = dirVector.Normalized();

	if (path.size() == 1)
	{
		_state = IDLE;
		_moveFps.sumTick = 0;
		_moveFps.lastTick = 0;
		return;
	}
	else
	{
		Vector3 nextPos = _pos + (_dir * moveDist);

		int32 nextX = static_cast<int32>(nextPos.x);
		int32 nextZ = static_cast<int32>(nextPos.z);

		Pos nextPPos = { nextX, nextZ };
		Pos nowPPos = { _pos.x, _pos.z };

		int32 nowX = static_cast<int32>(_pos.x);
		int32 nowZ = static_cast<int32>(_pos.z);

		int32 nextZIndex = nextZ / 32;
		int32 nextXIndex = nextX / 32;

		int32 nowZIndex = nowZ / 32;
		int32 nowXIndex = nowX / 32;

		if (nextXIndex != nowXIndex || nextZIndex != nowZIndex)
		{
			bool moveMonster = Map::GetInstance()->SpawnZones()[nextZIndex][nextXIndex]->Exist(_monsterId);
			if (moveMonster == false) // 몬스터가 Sector를 이동함
			{
				Map::GetInstance()->SpawnZones()[nowZIndex][nowXIndex]->Reset(this, nowPPos);
				Map::GetInstance()->SpawnZones()[nextZIndex][nextXIndex]->Set(this, nextPPos);
			}
		}

		std::cout << _moveFps.sumTick << std::endl;
		SyncMonsterPacket();
		_pos = nextPos;
		_moveFps.sumTick = 0;
	}
}

void Monster::Update_COOL_TIME()
{
	int32 currentTick = ::GetTickCount64();

	_coolTimeFps.lastTick = (_coolTimeFps.lastTick == 0 ? currentTick : _coolTimeFps.lastTick);

	int32 deltaTick = currentTick - _coolTimeFps.lastTick;

	_coolTimeFps.sumTick += deltaTick;

	_coolTimeFps.lastTick = currentTick;

	if (_coolTimeFps.sumTick < COOL_TIME_TICK)
		return;

	_state = State::IDLE;
	_coolTimeFps.lastTick = 0;
	_coolTimeFps.sumTick = 0;
}

void Monster::SyncMonsterPacket()
{
	if (_state == COOL_TIME)
		return;

	State state = _state;

	if (_state == PATROL || _state == TRACE)
		state = MOVE;

	BYTE sendBuffer[1024] = {};
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	int32 monsterId = _monsterId;
	Vector3 pos = _pos;
	float hp = _hp;
	Vector3 dir = _dir;
	
	bw.Write(state);
	bw.Write(monsterId);
	bw.Write(pos);
	bw.Write(hp);
	bw.Write(dir);

	pktHeader->_type = PacketProtocol::S2C_MONSTERSYNC;
	pktHeader->_pktSize = bw.GetWriterSize();

	Map::GetInstance()->BroadCast(_pos,sendBuffer,pktHeader->_pktSize);
}

void Monster::TraceSearch()
{
	std::vector<Pos> adjacent;
		 
	Map::GetInstance()->ConvertSectorIndexAddAdjacentrtSectorIndexAll(_pos, adjacent);

	for (auto item : adjacent)
	{
		Sector* sc = Map::GetInstance()->GetSector()[item.z][item.x];
		Vector3 monsterPos = _pos;

		for (auto session : sc->GetSessions())
		{
			Vector3 playerPos = session->GetPlayer()->GetPos();
			float dist = abs(monsterPos.x - playerPos.x) + abs(monsterPos.z - playerPos.z);

			if (dist > 10) continue;

			_target = session->GetPlayer();

			Vector3 dirVector = Vector3{ playerPos.x, _pos.y, playerPos.z } - _pos;
			_dir = dirVector.Normalized();

			_state = TRACE;
			return;
		}
	}
}

void Monster::AttackSearch()
{
	std::vector<std::vector<std::set<GameObject*>>>& mapDataGameObejct = MapDataManager::GetInstnace()->MapDataGameObject();

	int32 dx[9] = { 0,0,1,1,1,0,-1,-1,-1 };
	int32 dz[9] = { 0,-1,-1,0,1,1,1,0,-1 };

	int32 x = static_cast<int32>(_pos.x);
	int32 z = static_cast<int32>(_pos.z);

	for (int32 d = 0; d < 9; d++) 
	{
		int32 nx = x+dx[d];
		int32 nz = z+dz[d];

		if (MapDataManager::GetInstnace()->CanGo(nz, nx) == false) continue;

		if (mapDataGameObejct[nz][nx].size() == 0) continue;

		_state = State::ATTACK;
		auto it = mapDataGameObejct[nz][nx].begin();
		_target = (*it);
		break;
	}
}

void Monster::PatrolSearch()
{
	int32 currentTick = ::GetTickCount64();

	_patrolFps.lastTick = (_patrolFps.lastTick == 0 ? currentTick : _patrolFps.lastTick);

	int32 deltaTick = currentTick - _patrolFps.lastTick;

	_patrolFps.sumTick += deltaTick;

	_patrolFps.lastTick = currentTick;

	if (_patrolFps.sumTick < 5000)
		return;

	int32 x = static_cast<int32>(_pos.x) / 32;
	int32 z = static_cast<int32>(_pos.z) / 32;

	_patrolTarget = Map::GetInstance()->SpawnZones()[z][x]->RandomSpawnPos();

	_state = State::PATROL;
	_dir = (_patrolTarget - _pos).Normalized();

	_patrolFps.sumTick = 0;
	_patrolFps.lastTick = 0;
}
