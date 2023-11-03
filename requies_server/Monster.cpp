#include "pch.h"
#include "Monster.h"
#include "BufferWriter.h"
#include "Map.h"
#include "MapDataManager.h"
#include "Player.h"
#include "Sector.h"
#include "GameSession.h"
#include "SpawnZone.h"
Monster::Monster(int32 monsterId, MonsterType type, const Vector3& pos) : _monsterId(monsterId), _type(type), _state(STATE_NONE), GameObject(pos)
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

void Monster::Update_Idel()
{
	TraceSearch();
}

void Monster::Update_Attack()
{
	std::vector<std::vector<std::set<GameObject*>>>& mapDataGameObejct = MapDataManager::GetInstnace()->MapDataGameObject();

	Vector3 targetPos = _dir;

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

	_moveFps.lastTick = (_moveFps.lastTick == 0 ? currentTick : _moveFps.lastTick);

	int32 deltaTick = currentTick - _moveFps.lastTick;

	_moveFps.sumTick += deltaTick;

	_moveFps.lastTick = currentTick;

	if (_coolTimeFps.sumTick < MOVE_TICK)
		return;

	std::vector<Pos> path;
	Vector3 playerPos = _target->GetPos();
	MapDataManager::GetInstnace()->FindPath(playerPos, _pos, path);

	float nx = path[0].x + 0.5f;
	float nz = path[0].z + 0.5f;

	Vector3 dirVector = _pos - Vector3 {nx, _pos.y, nz};
	Vector3 dir = dirVector.Normalized();

	float distance = _speed * _moveFps.sumTick;
	Vector3 nextPos = _pos + (dir * distance);
	
	int32 nextX = static_cast<int32>(nextPos.x);
	int32 nextZ = static_cast<int32>(nextPos.z);

	int32 nowX = static_cast<int32>(_pos.x);
	int32 nowZ = static_cast<int32>(_pos.z);
	
	if (nextX != nowX || nextZ != nowZ) 
	{
		// TODO 문제 발생...
		// 몬스터가 플레이어를 쫒아다니다가, Secotr와 SpawnZone을 넘어가게 되면 문제가 발생함 
		// 1. 현재SpawnZone에 있는 MonsterDic에서 for문을 돌리고 있는데,몬스터가 이동하면 for문 내에서 MonsterDic을 수정해야함
		// MonsterList를 하나 빼서 관리를 할까요??? 
		// 
		// 2. 몬스터가 플레이어 추적하다가 Sector 단위로 넘어다니면 플레이어처럼 똑같이 처리해줘야 하는건가요?
		bool moveMonster = Map::GetInstance()->SpawnZones()[nextZ][nextX]->Exist(_monsterId);
		if (moveMonster)
		{
			// 몬스터도 Sector 이동하면 BroadCast를 해야하나요? 
		}
	}

	_pos = nextPos;
	SyncMonsterPacket();
	_moveFps.sumTick = 0;
}

void Monster::Update_Patrol()
{

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

	BYTE sendBuffer[1024] = {};
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	State state = _state;
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
	Pos pos = Map::GetInstance()->ConvertSectorIndex(_pos);
	Sector* sc = Map::GetInstance()->GetSector()[pos.z][pos.x];

	Vector3 monsterPos = _pos;

	for (auto session : sc->GetSessions())
	{
		Vector3 playerPos = session->GetPlayer()->GetPos();
		float dist = abs(monsterPos.x - playerPos.x) + abs(monsterPos.z - playerPos.z);

		if (dist > 10) continue;
	
		_target = session->GetPlayer();
		_dir = _target->GetPos();
		_state = TRACE;
		break;
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
		_dir = (*it)->GetPos();
		_target = (*it);
		break;
	}
}
