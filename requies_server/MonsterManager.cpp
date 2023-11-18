#include "pch.h"
#include "MonsterManager.h"
#include "Monster.h"
void MonsterManager::Update()
{
	MonsterUpdate();
}

void MonsterManager::MonsterUpdate()
{
	EnterCriticalSection(&_cs);

	for (auto it : _monsters) 
		it->Update();

	LeaveCriticalSection(&_cs);
}
