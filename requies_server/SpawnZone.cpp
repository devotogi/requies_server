#include "pch.h"
#include "SpawnZone.h"

SpawnZone::SpawnZone(int32 maxSpawnCount, const BoundBox& boundBox, MonsterType monsterType)
{
}

SpawnZone::~SpawnZone()
{
}

void SpawnZone::Update()
{
	int currentTick = ::GetTickCount64();
	
	int deltaTick = currentTick - _spawnFps.lastTick;
	_spawnFps.sumTick += deltaTick;

	_spawnFps.lastTick = currentTick;

	if (_spawnFps.sumTick >= SPAWN_TICK)
	{
		if (_monsterDic.size() < _maxSpawnCount) 
		{
			// TODO 몬스터 스폰
			
		}
		_spawnFps.sumTick = 0;
	}
}
