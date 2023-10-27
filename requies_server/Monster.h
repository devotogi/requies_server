#pragma once
class Monster
{
private:
	MonsterType _type;
	int32 _monsterId;
	Vector3 _pos;

public:
	Monster(int32 monsterId, MonsterType type);
	~Monster();

	int32 GetMonsterId() { return _monsterId; }
	MonsterType GetMonsterType() { return _type; }
	Vector3& GetPos() { return _pos; }
	void SetPos(const Vector3& pos) { _pos = pos; }
};

