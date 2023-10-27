#pragma once
class Sector;
class GameSession;
class SpawnZone;
#define SECTOR_SIZE 32
#define ADJACENT_COUNT 9
#define SECTOR_COUNT 8

class Map
{
private:
	int32 _maxX;
	int32 _maxZ;

	int32 _smaxX;
	int32 _smaxZ;

	std::vector<std::vector<Sector*>> _sectors;
	std::vector<std::vector<SpawnZone*>> _spawnZone;
	Vector3 _startPos[300][300];

public:
	static Map* GetInstance()
	{
		static Map instance(SECTOR_COUNT, SECTOR_COUNT);
		return &instance;
	}

public:
	Map(int32 maxX, int32 maxZ);
	~Map();

	void Set(GameSession* session);
	void Reset(GameSession* session);

	Pos ConvertSectorIndex(const Vector3& pos);
	Pos ConvertSectorIndex(const Pos& pos);
	void ConvertSectorIndexAddAdjacentrtSectorIndexAll(const Vector3& pos, std::vector<Pos>& out);
	void ConvertSectorIndexAddAdjacentrtSectorIndexAll(const Pos& pos, std::vector<Pos>& out);

	void MapSync(GameSession* session, const Vector3& prevPos, const Vector3& nowPos);
	void MapSyncAdjacentRemoveAndInsert(GameSession* session, const Vector3& prevPos, const Vector3& nowPos);
	void BroadCast(GameSession* session, BYTE* sendBuffer, int32 sendSize);
	void BroadCast(const Vector3& pos, BYTE* sendBuffer, int32 sendSize);
	void Update();
	Vector3 GetStartPos(int32 x, int32 z) { return _startPos[z][x];}
	void SetSMaxSize(int32 maxX, int32 maxZ) { _smaxX = maxX, _smaxZ = maxZ;  }

	std::vector<std::vector<SpawnZone*>>& SpawnZones() { return _spawnZone; }
};

