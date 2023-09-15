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

// 0�� // 16,16
// 1�� // 48,16
// 2�� // 80,16
// 3�� // 112,16
// 4�� // 144,16
// 5�� // 176,16
// 6�� // 208,16
// 7�� // 240,16
// 8�� // 16,32
// 9�� // 48,32
// 10�� // 80,32
// 11�� // 112,32
// 12�� // 144,32
// 13�� // 176,32
// 14�� // 208,32
// 15�� // 240,32
// 16�� // 16,32
// 17�� // 48,32
// 18�� // 80,32
// 19�� // 112,32
// 20�� // 144,32
// 21�� // 176,32
// 22�� // 208,32
// 23�� // 240,32

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
		Reset(session, prevPos);
		_map[nowMapIndex.z][nowMapIndex.x]->Set(session);
		Set(session, nowPos);
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

	int posX = static_cast<int>(pos.x);
	int posZ = static_cast<int>(pos.z);

	Pos sectorIndex = { posX / sectorSize ,posZ / sectorSize };

	return sectorIndex;
}

Pos MapManager::ConvertSectorIndex(const Pos& pos)
{
	const int sectorSize = 32;

	int posX = static_cast<int>(pos.x);
	int posZ = static_cast<int>(pos.z);

	Pos sectorIndex = { posX / sectorSize ,posZ / sectorSize };

	return sectorIndex;
}


void MapManager::ConvertSectorIndex(const Vector3& pos, std::vector<Pos>& out)
{
	// ��, ������, ����, �����Ʒ�,�Ʒ�,�޾Ʒ�,��,����

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
	// �ڱ��ڽ�,��, ������, ����, �����Ʒ�,�Ʒ�,�޾Ʒ�,��,����
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