#include "pch.h"
#include "Map.h"
#include "GameSession.h"
#include "Player.h"
#include "BufferWriter.h"
#include "Sector.h"
#include "SpawnZone.h"
#include "MapDataManager.h"
#include "MonsterManager.h"
// 자기자신, 위, 오른위, 오른, 오른아래, 아래, 왼아래, 왼, 왼위
int32 dx[9] = { 0,0,1,1,1,0,-1,-1,-1 };
int32 dz[9] = { 0,-1,-1,0,1,1,1,0,-1 };

Map::Map(int32 maxX, int32 maxZ) : _maxX(maxX), _maxZ(maxZ)
{
	_sectors.resize(maxZ);

	for (int i = 0; i < _maxZ; i++)
		_sectors[i].resize(_maxX);

	for (int z = 0; z < _maxZ; z++)
	{
		for (int x = 0; x < _maxX; x++)
		{
			_sectors[z][x] = new Sector();
		}
	}

	int32 xPos = SECTOR_SIZE / 2;
	int32 zPos = SECTOR_SIZE / 2;

	for (int z = 0; z < SECTOR_COUNT; z++)
	{
		for (int x = 0; x < SECTOR_COUNT; x++)
		{
			_startPos[z][x] = { static_cast<float>(xPos),0,static_cast<float>(zPos) };
			xPos += SECTOR_SIZE;
			xPos %= SECTOR_SIZE * _maxX;
		}
		zPos += SECTOR_SIZE;
		zPos %= SECTOR_SIZE * _maxZ;
	}
}

Map::~Map()
{
	for (int z = 0; z < SECTOR_COUNT; z++)
	{
		for (int x = 0; x < SECTOR_COUNT; x++)
		{
			if (_sectors[z][x])
				delete _sectors[z][x];
		}
	}
}

void Map::Set(GameSession* session)
{
	Vector3 nowPos = session->GetPlayer()->GetPos();
	Pos sectorIndex = ConvertSectorIndex(nowPos);

	Sector* sector = _sectors[sectorIndex.z][sectorIndex.x];
	sector->Set(session);

	std::vector<Pos> adjacent;
	ConvertSectorIndexAddAdjacentrtSectorIndexAll(nowPos, adjacent);

	for (auto item : adjacent)
	{
		_sectors[item.z][item.x]->SendPlayerList(session);
		_spawnZone[item.z][item.x]->SendMonsterList(session);
	}
}

void Map::Reset(GameSession* session)
{
	Vector3 nowPos = session->GetPlayer()->GetPos();
	Pos sectorIndex = ConvertSectorIndex(nowPos);

	Sector* sector = _sectors[sectorIndex.z][sectorIndex.x];
	sector->Reset(session);

	std::vector<Pos> adjacent;
	ConvertSectorIndexAddAdjacentrtSectorIndexAll(nowPos, adjacent);

	for (auto item : adjacent)
	{
		_sectors[item.z][item.x]->SendRemoveList(session);
		_spawnZone[item.z][item.x]->SendRemoveList(session);
	}
}

Pos Map::ConvertSectorIndex(const Vector3& pos)
{
	int32 posX = static_cast<int32>(pos.x);
	int32 posZ = static_cast<int32>(pos.z);

	Pos sectorIndex = { posX / SECTOR_SIZE ,posZ / SECTOR_SIZE };

	return sectorIndex;
}

Pos Map::ConvertSectorIndex(const Pos& pos)
{
	int32 posX = static_cast<int32>(pos.x);
	int32 posZ = static_cast<int32>(pos.z);

	Pos sectorIndex = { posX / SECTOR_SIZE ,posZ / SECTOR_SIZE };

	return sectorIndex;
}

void Map::ConvertSectorIndexAddAdjacentrtSectorIndexAll(const Vector3& pos, std::vector<Pos>& out)
{
	Pos here = ConvertSectorIndex(pos);
	for (int i = 0; i < ADJACENT_COUNT; i++)
	{
		int32 nextX = here.x + dx[i];
		int32 nextZ = here.z + dz[i];

		if (nextX < 0 || nextX > ADJACENT_COUNT - 2 || nextZ < 0 || nextZ > ADJACENT_COUNT - 2)
			continue;

		out.push_back({ nextX,nextZ });
	}
}

void Map::ConvertSectorIndexAddAdjacentrtSectorIndexAll(const Pos& pos, std::vector<Pos>& out)
{
	Pos here = ConvertSectorIndex(pos);
	for (int i = 0; i < ADJACENT_COUNT; i++)
	{
		int32 nextX = here.x + dx[i];
		int32 nextZ = here.z + dz[i];

		if (nextX < 0 || nextX > ADJACENT_COUNT - 2 || nextZ < 0 || nextZ > ADJACENT_COUNT - 2)
			continue;

		out.push_back({ nextX,nextZ });
	}
}

void Map::BroadCast(GameSession* session, BYTE* sendBuffer, int32 sendSize)
{
	std::vector<Pos> adjacent;
	Vector3 nowPos = session->GetPlayer()->GetPos();

	ConvertSectorIndexAddAdjacentrtSectorIndexAll(nowPos, adjacent);
	for (auto item : adjacent)
		_sectors[item.z][item.x]->BroadCast(session, sendBuffer, sendSize);
}

void Map::BroadCast(const Vector3& pos, BYTE* sendBuffer, int32 sendSize)
{
	std::vector<Pos> adjacent;
	ConvertSectorIndexAddAdjacentrtSectorIndexAll(pos, adjacent);
	
	for (auto item : adjacent)
		_sectors[item.z][item.x]->BroadCast(nullptr, sendBuffer, sendSize);
}

void Map::Update()
{
	for (int32 z = 0; z < _smaxZ; z++)
	{
		for (int32 x = 0; x < _smaxX; x++)
		{
			_spawnZone[z][x]->Update();
		}
	}

	MonsterManager::GetInstnace()->Update();
}

void Map::AttackedMonster(const Vector3& pos, int32 monsterId, int32 damage)
{
	Pos here = ConvertSectorIndex(pos);
	_spawnZone[here.z][here.x]->AttackedMonster(monsterId, damage);
}

void Map::MapSync(GameSession* session, const Vector3& prevPos, const Vector3& nowPos)
{
	Pos prevMapIndex = ConvertSectorIndex(prevPos);
	Pos nowMapIndex = ConvertSectorIndex(nowPos);

	session->GetPlayer();
	MapDataManager::GetInstnace()->ApplyMapGameObject(session->GetPlayer(),prevPos, false);
	MapDataManager::GetInstnace()->ApplyMapGameObject(session->GetPlayer(),nowPos, true);

	if (prevMapIndex != nowMapIndex)
	{
		_sectors[prevMapIndex.z][prevMapIndex.x]->Reset(session);
		_sectors[nowMapIndex.z][nowMapIndex.x]->Set(session);

		MapSyncAdjacentRemoveAndInsert(session, prevPos, nowPos);
	}
}

void Map::MapSyncAdjacentRemoveAndInsert(GameSession* session, const Vector3& prevPos, const Vector3& nowPos)
{
	std::vector<Pos> prevs;
	ConvertSectorIndexAddAdjacentrtSectorIndexAll(prevPos, prevs);
	std::vector<Pos> nows;
	ConvertSectorIndexAddAdjacentrtSectorIndexAll(nowPos, nows);

	std::set<Pos> nowSet;
	for (auto now : nows)
		nowSet.insert(now);

	std::set<Pos> prevSet;
	for (auto prev : prevs)
		prevSet.insert(prev);

	std::vector<Pos>  remove;
	std::vector<Pos>  adds;

	for (auto prev : prevs)
	{
		if (!nowSet.count(prev))
			remove.push_back(prev);
	}

	for (auto now : nows)
	{
		if (!prevSet.count(now))
			adds.push_back(now);
	}

	for (auto item : remove)
	{
		_sectors[item.z][item.x]->SendRemoveList(session);
		_spawnZone[item.z][item.x]->SendRemoveList(session);
	}

	for (auto item : adds)
	{
		_sectors[item.z][item.x]->SendPlayerList(session);
		_spawnZone[item.z][item.x]->SendMonsterList(session);
	}
}