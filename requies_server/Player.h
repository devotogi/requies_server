#pragma once
class GameSession;

class Player
{
private:
	int32 _sessionId;
	Vector3	_pos;
	Vector3	_prevPos;
	State _state;
	Dir	_dir;
	Dir	_mouseDir;
	Quaternion _cameraLocalRotation;
	CRITICAL_SECTION _cs;
	GameSession* _session;
	Vector3 _target;
public:
	Player(GameSession* session, int32 sessionId, const Vector3& pos);
	~Player();

	Vector3&	GetPos() { return _pos; }
	Vector3&	GetPrevPos() { return _prevPos; }
	State		GetState() { return _state; }
	Dir			GetDir() { return _dir; }
	Dir			GetMouseDir() { return _mouseDir; }
	Quaternion& GetCameraLocalRotation() { return _cameraLocalRotation; }
	Vector3& GetTarget() { return _target; }
	void		SetPrevPos(const Vector3& prevPos) { _prevPos = prevPos; }
	void		PlayerSync(const Vector3& pos, State state, Dir dir, Dir mousedir, const Quaternion& cameraLocalRotation, const Vector3& target);
	void        Attacked();
};