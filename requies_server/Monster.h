#pragma once
#include "GameObject.h"
class Monster : public GameObject
{
	enum : int32
	{
		ATTACKED_TICK = 600,
		COOL_TIME_TICK = 1000,
		MOVE_TICK = 1000,
		SEARCH_TICK = 1000,
		PATROL_TICK = 2000,
	};

private:
	Pos _spawnZoneIndex;
	MonsterType _type;
	State _state;
	int32 _monsterId;
	int32 _hp = 1000;
	Vector3 _patrolTarget;
	Vector3 _dir;
	int32 _playerId;
	GameObject* _target;
	int32 _damage = 100;

	FPS _attackedFps;
	FPS _coolTimeFps;
	FPS _moveFps;
	FPS _searchFps;
	FPS _patrolFps;

	float _speed = 2.5f;

public:
	Monster(const Pos& spawnZoneIndex, int32 monsterId, MonsterType type, const Vector3& pos);
	~Monster();

	int32 GetMonsterId() { return _monsterId; }
	float GetMonsterHp() { return _hp; }
	MonsterType GetMonsterType() { return _type; }
	State GetState() { return _state; }
	Vector3& GetPos() { return _pos; }
	void SetPos(const Vector3& pos) { _pos = pos; }
	bool Attacked(int32 damage);
	Vector3& GetDir() { return _dir; }
	void Update();
	void Dead();
private:
	void Update_Idel();
	void Update_Attack();
	void Update_Attacked();
	void Update_Trace();
	void Update_Patrol();
	void Update_COOL_TIME();
	void SyncMonsterPacket();

	void TraceSearch();
	void AttackSearch();
	void PatrolSearch();
};

