#include "pch.h"
#include "Monster.h"

Monster::Monster(int32 monsterId, MonsterType type) : _monsterId(monsterId), _type(type)
{
}

Monster::~Monster()
{
}

bool Monster::Attacked(int32 damage)
{
	_hp -= damage;

	if (_hp <= 0) 
	{
		_hp = 0;
		return true;
	}

	return false;
}
