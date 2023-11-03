#include "pch.h"
#include "MonsterManager.h"
#include "Monster.h"
void MonsterManager::Update()
{
	int currentTick = ::GetTickCount64();

	int monsterUpdateDeltaTick = currentTick - _monsterUpdateFps.lastTick;
	_monsterUpdateFps.sumTick += monsterUpdateDeltaTick;

	_monsterUpdateFps.lastTick = currentTick;


	if (_monsterUpdateFps.sumTick >= MONSTERUPDATE_TICK)
	{
		MonsterUpdate();
		_monsterUpdateFps.sumTick = 0;
	}
}

void MonsterManager::MonsterUpdate()
{
	EnterCriticalSection(&_cs);

	for (auto it : _monsters) 
		it->Update();

	LeaveCriticalSection(&_cs);
}
