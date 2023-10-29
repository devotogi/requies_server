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
	void AttackedMonster(int32 monsterId, int32 damage);
private:
	void AddMonster(int32 monsterKey, Monster* monster);
	void RemoveMonster(int32 monsterKey);
	void Spawn();
	Vector3 RandomSpawnPos();
	void SendMonsterSpawn(Monster* monster);
};

