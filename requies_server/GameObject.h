#pragma once
class GameObject
{
protected:
	Vector3 _pos;

public:
	GameObject(const Vector3& pos) : _pos(pos) {};
	virtual ~GameObject() {};

	Vector3& GetPos() { return _pos; }
};

