#pragma once
class Monster;
class GameSession;

class SpawnZone
{
	enum
	{
		SPAWN_TICK = 10000,
	};

private:
	Pos							 _spawnZoneIndex;
	int32						 _maxSpawnCount = 0;
	BoundBox					 _boundBox;
	FPS							 _spawnFps;
	FPS							 _monsterUpdateFps;
	MonsterType					 _monsterType;
	std::map<int32, Monster*>    _monsterDic;
	CRITICAL_SECTION			 _cs;
	int32						 _spawnCnt = 0;
public:
	SpawnZone(const Pos& spawnZoneIndex, int32 maxSpawnCount, const BoundBox& boundBox, MonsterType monsterType);
	~SpawnZone();
	
	void Update();
	void SendMonsterList(GameSession* session);
	void SendRemoveList(GameSession* session);
	void AttackedMonster(int32 monsterId, int32 damage);
	Vector3 RandomSpawnPos();
	bool Exist(int32);

	void Reset(Monster* monster, const Pos& nowPos);
	void Set(Monster* monster, const Pos& nowPos);
	void RemoveMonster(int32 monsterKey);
private:
	void AddMonster(int32 monsterKey, Monster* monster);

	void Spawn();

	void SendMonsterSpawn(Monster* monster);
};

