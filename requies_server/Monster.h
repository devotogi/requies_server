#pragma once
#include "GameObject.h"
class Monster : public GameObject
{
	enum : int32
	{
		ATTACKED_TICK = 600,
		COOL_TIME_TICK = 1000
	};

private:
	MonsterType _type;
	State _state;
	int32 _monsterId;
	int32 _hp = 1000;
	
	Vector3 _dir;
	int32 _playerId;
	GameObject* _target;
	int32 _damage = 100;

	FPS _attackedFps;
	FPS _coolTimeFps;

public:
	Monster(int32 monsterId, MonsterType type, const Vector3& pos);
	~Monster();

	int32 GetMonsterId() { return _monsterId; }
	float GetMonsterHp() { return _hp; }
	MonsterType GetMonsterType() { return _type; }
	State GetState() { return _state; }
	Vector3& GetPos() { return _pos; }
	void SetPos(const Vector3& pos) { _pos = pos; }
	bool Attacked(int32 damage);

	void Update();

private:
	void Update_Idel();
	void Update_Move();
	void Update_Attack();
	void Update_Attacked();
	void Update_COOL_TIME();
	void SyncMonsterPacket();

};

