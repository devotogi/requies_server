#include "pch.h"
#include "MapManager.h"
#include "GameSession.h"
#include "Player.h"
#include "BufferWriter.h"
#include "Sector.h"
MapManager* MapManager::_instance = nullptr;

MapManager::MapManager()
{
	_map.resize(8);

	for (int i = 0; i < 8; i++)
		_map[i].resize(8);

	for (int z = 0; z < 8; z++)
	{
		for (int x = 0; x < 8; x++)
		{
			_map[z][x] = new Sector();
		}
	}

	int xPos = 16;
	int zPos = 16;

	for (int z = 0; z < 8; z++)
	{
		for (int x = 0; x < 8; x++)
		{
			_startPos[z][x] = { static_cast<float>(xPos),0,static_cast<float>(zPos) };
			xPos += 32;
			xPos %= 256;
		}
		zPos += 32;
		zPos %= 256;
	}
}

MapManager::~MapManager()
{
	for (int z = 0; z < 8; z++)
	{
		for (int x = 0; x < 8; x++)
		{
			if (_map[z][x])
				delete _map[z][x];
		}
	}
}

void MapManager::BroadCast(GameSession* session, BYTE* sendBuffer, int32 sendSize)
{
	std::vector<Pos> adjacent;
	ConvertSectorIndex(session, adjacent);
	for (auto item : adjacent)
		_map[item.z][item.x]->BroadCast(session, sendBuffer, sendSize);
}

void MapManager::Sync(GameSession* session, const Vector3& prevPos, const Vector3& nowPos)
{
	Pos prevMapIndex = ConvertSectorIndex(prevPos);
	Pos nowMapIndex = ConvertSectorIndex(nowPos);

	if (prevMapIndex != nowMapIndex)
	{
		_map[prevMapIndex.z][prevMapIndex.x]->Reset(session);
		_map[nowMapIndex.z][nowMapIndex.x]->Set(session);

		SyncProc(session, prevPos, nowPos);
	}
}

void MapManager::SyncProc(GameSession* session, const Vector3& prevPos, const Vector3& nowPos)
{
	std::vector<Pos> prevs;
	ConvertSectorIndex(prevPos, prevs);
	std::vector<Pos> nows;
	ConvertSectorIndex(nowPos, nows);
	
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
		_map[item.z][item.x]->SendRemoveList(session);
	}

	for (auto item : adds)
	{
		_map[item.z][item.x]->SendPlayerList(session);
	}
}

void MapManager::Set(GameSession* session)
{
	Player* player = session->GetPlayer();
	Pos sectorIndex = ConvertSectorIndex(session);

	Sector* sector = _map[sectorIndex.z][sectorIndex.x];
	sector->Set(session);

	std::vector<Pos> adjacent;
	ConvertSectorIndex(player->GetPos(), adjacent);

	for (auto item : adjacent)
	{
		_map[item.z][item.x]->SendPlayerList(session);
	}
}

void MapManager::Reset(GameSession* session)
{
	Player* player = session->GetPlayer();
	Pos sectorIndex = ConvertSectorIndex(session);

	Sector* sector = _map[sectorIndex.z][sectorIndex.x];
	sector->Reset(session);

	std::vector<Pos> adjacent;
	ConvertSectorIndex(player->GetPos(), adjacent);

	for (auto item : adjacent)
	{
		_map[item.z][item.x]->SendRemoveList(session);
	}
}

void MapManager::Set(GameSession* session, const Vector3& pos)
{
	std::vector<Pos> adjacent;
	ConvertSectorIndex(pos, adjacent);

	for (auto item : adjacent)
	{
		_map[item.z][item.x]->SendPlayerList(session);
	}
}

void MapManager::Reset(GameSession* session, const Vector3& pos)
{
	std::vector<Pos> adjacent;
	ConvertSectorIndex(pos, adjacent);

	for (auto item : adjacent)
	{
		_map[item.z][item.x]->SendRemoveList(session);
	}
}

Pos MapManager::ConvertSectorIndex(GameSession* session)
{
	Player* player = session->GetPlayer();
	Vector3 pos = player->GetPos();
	return ConvertSectorIndex(pos);
}

void MapManager::ConvertSectorIndex(GameSession* session, std::vector<Pos>& out)
{
	Player* player = session->GetPlayer();
	Vector3 pos = player->GetPos();
	ConvertSectorIndex(pos, out);
}

Pos MapManager::ConvertSectorIndex(const Vector3& pos)
{
	const int sectorSize = 32;

	int32 posX = static_cast<int>(pos.x);
	int32 posZ = static_cast<int>(pos.z);

	Pos sectorIndex = { posX / sectorSize ,posZ / sectorSize };

	return sectorIndex;
}

Pos MapManager::ConvertSectorIndex(const Pos& pos)
{
	const int sectorSize = 32;

	int32 posX = static_cast<int>(pos.x);
	int32 posZ = static_cast<int>(pos.z);

	Pos sectorIndex = { posX / sectorSize ,posZ / sectorSize };

	return sectorIndex;
}


void MapManager::ConvertSectorIndex(const Vector3& pos, std::vector<Pos>& out)
{
	// 위, 오른위, 오른, 오른아래,아래,왼아래,왼,왼위
	int32 dx[9] = { 0,0,1,1,1,0,-1,-1,-1 };
	int32 dz[9] = { 0,-1,-1,0,1,1,1,0,-1 };
	Pos here = ConvertSectorIndex(pos);
	for (int i = 0; i < 9; i++)
	{
		int32 nextX = here.x + dx[i];
		int32 nextZ = here.z + dz[i];

		if (nextX < 0 || nextX > 7 || nextZ < 0 || nextZ > 7)
			continue;

		out.push_back({ nextX,nextZ });
	}
}

void MapManager::ConvertSectorIndex(const Pos& pos, std::vector<Pos>& out)
{
	// 자기자신,위, 오른위, 오른, 오른아래,아래,왼아래,왼,왼위
	int32 dx[9] = { 0,0,1,1,1,0,-1,-1,-1 };
	int32 dz[9] = { 0,-1,-1,0,1,1,1,0,-1 };
	Pos here = ConvertSectorIndex(pos);
	for (int i = 0; i < 9; i++)
	{
		int32 nextX = here.x + dx[i];
		int32 nextZ = here.z + dz[i];

		if (nextX < 0 || nextX > 7 || nextZ < 0 || nextZ > 7)
			continue;

		out.push_back({ nextX,nextZ });
	}
}