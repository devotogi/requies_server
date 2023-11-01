#pragma once
#include "GameObject.h"
class GameSession;

class Player : public GameObject
{
private:
	int32 _sessionId;
	Vector3	_prevPos;
	State _state;
	Dir	_dir;
	Dir	_mouseDir;
	Quaternion _cameraLocalRotation;
	CRITICAL_SECTION _cs;
	GameSession* _session;
	Vector3 _target;
	MoveType _moveType;
	Quaternion _localRotation;
	float _hp = 1000;
	float _mp = 1000;
	bool _death = false;
public:
	Player(GameSession* session, int32 sessionId, const Vector3& pos);
	~Player();

	Vector3&	GetPos() { return _pos; }
	Vector3&	GetPrevPos() { return _prevPos; }
	State		GetState() { return _state; }
	Dir			GetDir() { return _dir; }
	Dir			GetMouseDir() { return _mouseDir; }
	Quaternion& GetCameraLocalRotation() { return _cameraLocalRotation; }
	MoveType	GetMoveType() { return _moveType; }
	Vector3&	GetTarget() { return _target; }
	Quaternion&	GetLocalRtation() { return _localRotation; }
	float		GetHp() { return _hp; }
	float		GetMp() { return _mp; }
	void		SetPrevPos(const Vector3& prevPos) { _prevPos = prevPos; }
	void		PlayerSync(const Vector3& pos, State state, Dir dir, Dir mousedir, const Quaternion& cameraLocalRotation, const Vector3& target, MoveType moveType, Quaternion localRotation);
	void        Attacked(int damage);
	void        ReSpawn();
};