#pragma once
class Monster;
class GameSession;

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
	CRITICAL_SECTION			 _cs;
public:
	SpawnZone(int32 maxSpawnCount, const BoundBox& boundBox, MonsterType monsterType);
	~SpawnZone();
	
	void Update();
	void SendMonsterList(GameSession* session);
	void SendRemoveList(GameSession* session);

private:
	void AddMonster(int32 monsterKey, Monster* monster);
	void Spawn();
	Vector3 RandomSpawnPos();
	void SendMonsterSpawn(Monster* monster);
};

