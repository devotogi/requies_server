#pragma once
class Monster
{
	enum : int32
	{
		ATTACKED_TICK = 600
	};

private:
	MonsterType _type;
	State _state;
	int32 _monsterId;
	Vector3 _pos;
	int32 _hp = 1000;

	FPS _attackedFps;
public:
	Monster(int32 monsterId, MonsterType type);
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
	void SyncMonsterPacket();

};

