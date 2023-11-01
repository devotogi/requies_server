#include "pch.h"
#include "SpawnZone.h"
#include "MapDataManager.h"
#include "Monster.h"
#include "Map.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "GameSession.h"
SpawnZone::SpawnZone(int32 maxSpawnCount, const BoundBox& boundBox, MonsterType monsterType) : _maxSpawnCount(maxSpawnCount), _boundBox(boundBox), _monsterType(monsterType)
{
	InitializeCriticalSection(&_cs);
}

SpawnZone::~SpawnZone()
{
	DeleteCriticalSection(&_cs);
}

void SpawnZone::Update()
{
	int currentTick = ::GetTickCount64();
	
	int deltaTick = currentTick - _spawnFps.lastTick;
	_spawnFps.sumTick += deltaTick;

	_spawnFps.lastTick = currentTick;
/*
	if (_spawnFps.sumTick < SPAWN_TICK) return;
	if (_monsterDic.size() >= _maxSpawnCount) return;

	Spawn();
	_spawnFps.sumTick = 0;
	*/
	if (_spawnFps.sumTick >= SPAWN_TICK)
	{
		if (_monsterDic.size() < _maxSpawnCount) 
		{
			Spawn();
		}
		_spawnFps.sumTick = 0;
	}

	int monsterUpdateDeltaTick = currentTick - _monsterUpdateFps.lastTick;
	_monsterUpdateFps.sumTick += monsterUpdateDeltaTick;

	_monsterUpdateFps.lastTick = currentTick;


	if (_monsterUpdateFps.sumTick >= MONSTERUPDATE_TICK)
	{
		MonsterUpdate();
		_monsterUpdateFps.sumTick = 0;
	}
}

void SpawnZone::AddMonster(int32 monsterKey, Monster* monster)
{
	EnterCriticalSection(&_cs);
	_monsterDic.insert({ monsterKey, monster });
	LeaveCriticalSection(&_cs);
}

void SpawnZone::RemoveMonster(int32 monsterKey)
{
	EnterCriticalSection(&_cs);
	_monsterDic.erase( monsterKey );
	LeaveCriticalSection(&_cs);
}

void SpawnZone::MonsterUpdate()
{
	EnterCriticalSection(&_cs);
	for (auto monsterDic : _monsterDic)
	{
		int32 monsterId = monsterDic.first;
		Monster* monster = monsterDic.second;

		monster->Update();
	}
	LeaveCriticalSection(&_cs);
}

void SpawnZone::Spawn()
{
	// TODO 몬스터 스폰
	int32 nowMonsterCnt = _monsterDic.size();
	int32 spawnMonsterCnt = _maxSpawnCount - nowMonsterCnt;

	for (int32 i = 0; i < spawnMonsterCnt; i++) 
	{
		int32 newMonsterId = MapDataManager::GetInstnace()->PopMonsterId();
		Vector3 monsterPos = RandomSpawnPos();
		Monster* monster = new Monster(newMonsterId, MonsterType::Bear, monsterPos);
		AddMonster(newMonsterId, monster);
		SendMonsterSpawn(monster);
	}
}

Vector3 SpawnZone::RandomSpawnPos()
{
	thread_local std::mt19937 generator(std::random_device{}());
	bool canSpawn = false;
	int32 x = 0;
	int32 z = 0;

	while (!canSpawn) 
	{
		std::uniform_int_distribution<int> xPos(_boundBox.minX, _boundBox.maxX - 1);
		std::uniform_int_distribution<int> zPos(_boundBox.minZ, _boundBox.maxZ - 1);

		x = xPos(generator);
		z = zPos(generator);

		canSpawn = MapDataManager::GetInstnace()->CanGo(z, x);
	}

	Vector3 v3 = { static_cast<float>(x), 0.f, static_cast<float>(z) };

	return v3;
}

void SpawnZone::SendMonsterSpawn(Monster* monster)
{
	Vector3 monsterV3 = monster->GetPos();

	BYTE sendBuffer[256] = {};
	BufferWriter bw(sendBuffer);

	PacketHeader* header = bw.WriteReserve<PacketHeader>();
	bw.Write(monster->GetMonsterId());
	bw.Write(monster->GetMonsterType());
	bw.Write(monster->GetPos());
	bw.Write(monster->GetMonsterHp());

	header->_pktSize = bw.GetWriterSize();
	header->_type = S2C_MONSTERSPAWN;

	Map::GetInstance()->BroadCast(monsterV3, sendBuffer, header->_pktSize);
}

void SpawnZone::SendRemoveList(GameSession* session)
{
	BYTE sendBuffer[4096] = {};
	BufferWriter bw(sendBuffer);
	
	EnterCriticalSection(&_cs);
	
	PacketHeader* header = bw.WriteReserve<PacketHeader>();
	const int32 cnt = _monsterDic.size();
	
	bw.Write(cnt);

	for (auto monsterDic : _monsterDic) 
	{	
		int32 monsterKey = monsterDic.first;
		Monster* monster = monsterDic.second;

		bw.Write(monster->GetMonsterId());
	}

	LeaveCriticalSection(&_cs);

	header->_pktSize = bw.GetWriterSize();
	header->_type = S2C_MONSTERREMOVELIST;

	session->Send(sendBuffer, bw.GetWriterSize());
}

void SpawnZone::AttackedMonster(int32 monsterId, int32 damage)
{
	auto it = _monsterDic.find(monsterId);

	if (it == _monsterDic.end()) return;
	bool death = false;
	Monster* attackedMonster = nullptr;
	Vector3 monsterPos;
	int32 _monsterId;
	int32 monsterType;
	float monsterHp;
	State monsterState;
	EnterCriticalSection(&_cs);
	
	attackedMonster = it->second;
	death = attackedMonster->Attacked(damage);

	_monsterId = attackedMonster->GetMonsterId();
	monsterPos = attackedMonster->GetPos();
	monsterType = attackedMonster->GetMonsterType();
	monsterHp = attackedMonster->GetMonsterHp();
	monsterState = attackedMonster->GetState();

	LeaveCriticalSection(&_cs);
	
	if (attackedMonster == nullptr) return;

	BYTE sendBuffer[4096] = {};
	BufferWriter bw(sendBuffer);

	PacketHeader* header = bw.WriteReserve<PacketHeader>();
	bw.Write(monsterState);
	bw.Write(monsterType);
	bw.Write(_monsterId);
	bw.Write(monsterPos);
	bw.Write(monsterHp);
	header->_pktSize = bw.GetWriterSize();
	header->_type = S2C_MONSTERATTACKED;

	Map::GetInstance()->BroadCast(monsterPos, sendBuffer, header->_pktSize);

	if (!death) return;
	
	RemoveMonster(_monsterId);

	EnterCriticalSection(&_cs);
	delete attackedMonster;

	MapDataManager::GetInstnace()->PushMonsterId(_monsterId);
	LeaveCriticalSection(&_cs);

	{
		BYTE sendBuffer[4096] = {};
		BufferWriter bw(sendBuffer);

		PacketHeader* header = bw.WriteReserve<PacketHeader>();
		bw.Write(_monsterId);

		header->_pktSize = bw.GetWriterSize();
		header->_type = S2C_MONSTERDEAD;
		Map::GetInstance()->BroadCast(monsterPos, sendBuffer, header->_pktSize);
	}
}

void SpawnZone::SendMonsterList(GameSession* session)
{
	BYTE sendBuffer[4096] = {};
	BufferWriter bw(sendBuffer);

	EnterCriticalSection(&_cs);

	PacketHeader* header = bw.WriteReserve<PacketHeader>();
	const int32 cnt = _monsterDic.size();

	bw.Write(cnt);

	for (auto monsterDic : _monsterDic)
	{
		int32 monsterKey = monsterDic.first;
		Monster* monster = monsterDic.second;

		bw.Write(monster->GetMonsterId());
		bw.Write(monster->GetMonsterType());
		bw.Write(monster->GetPos());
		bw.Write(monster->GetMonsterHp());

	}

	LeaveCriticalSection(&_cs);

	header->_pktSize = bw.GetWriterSize();
	header->_type = S2C_MONSTERRENEWLIST;

	session->Send(sendBuffer, bw.GetWriterSize());
}
