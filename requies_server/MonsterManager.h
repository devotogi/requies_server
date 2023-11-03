#pragma once
class Monster;

class MonsterManager
{
	enum
	{
		MONSTERUPDATE_TICK = 20
	};

private:
	std::set<Monster*> _monsters;
	CRITICAL_SECTION _cs;
	FPS _monsterUpdateFps;
public:
	static MonsterManager* GetInstnace()
	{
		static MonsterManager _monsterManger;
		return &_monsterManger;
	}

	MonsterManager()
	{
		InitializeCriticalSection(&_cs);
	}

	~MonsterManager()
	{
		DeleteCriticalSection(&_cs);
	}

	void AddMonster(Monster* monster)
	{
		EnterCriticalSection(&_cs);
		_monsters.insert(monster);
		LeaveCriticalSection(&_cs);
	}

	void RemoveMonster(Monster* monster)
	{
		EnterCriticalSection(&_cs);
		_monsters.erase(monster);
		delete monster;
		LeaveCriticalSection(&_cs);
	}

	void Update();

	void MonsterUpdate();
};

