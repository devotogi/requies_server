#pragma once
class Monster;

class SpawnZone
{
	enum
	{
		SPAWN_TICK = 1000
	};

private:
	int32						 _maxSpawnCount = 0;
	BoundBox					 _boundBox;
	FPS							 _spawnFps;
	MonsterType					 _monsterType;
	std::map<int32, Monster*>    _monsterDic;

public:
	SpawnZone(int32 maxSpawnCount, const BoundBox& boundBox, MonsterType monsterType);
	~SpawnZone();
	
	void Update();
};

