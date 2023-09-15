#pragma once
class GameSession;
class Sector;
class MapManager
{
private:
	static MapManager* _instance;
	std::vector<std::vector<Sector*>>  _map;
	Vector3 _startPos[8][8];

public:
	MapManager();
	~MapManager();
	/*
	static MapManager* GetInstance()
	{
		static MapManager	_instance;
		return &_instance;
	}
	*/

	static MapManager* GetInstance()
	{
		if (_instance == nullptr)
			_instance = new MapManager();

		return _instance;
	}

	void Set(GameSession* session);
	void Reset(GameSession* session);
	void Set(GameSession* session, const Vector3& pos);
	void Reset(GameSession* session, const Vector3& pos);

	void ConvertSectorIndex(GameSession* session, std::vector<Pos>& out);
	Pos ConvertSectorIndex(GameSession* session);
	Pos ConvertSectorIndex(const Vector3& pos);
	Pos ConvertSectorIndex(const Pos& pos);
	void ConvertSectorIndex(const Vector3& pos, std::vector<Pos>& out);
	void ConvertSectorIndex(const Pos& pos, std::vector<Pos>& out);
	void BroadCast(GameSession* session, BYTE* sendBuffer, int32 sendSize);
	void Sync(GameSession* session, const Vector3& prevPos, const Vector3& nowPos);
	Vector3 GetStartPos(int32 x, int32 z) {
		return _startPos
			[z][x];
	}
};

