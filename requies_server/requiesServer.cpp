#include "pch.h"
#include "ServerService.h"
#include "GameSession.h"
#include "ThreadManager.h"
#include "IOCPCore.h"
#include "MapDataManager.h"
#include "SpawnZone.h"
#include "Map.h"

void Init() 
{
	std::cout << "MapData Parse Start" << std::endl;

	FILE* fp = NULL;
	// C:\Users\jgkan\Desktop\map
	fopen_s(&fp,"C:\\Users\\jgkan\\Desktop\\map\\map.dat", "r");
	
	fseek(fp, 0, SEEK_END);
	const int32 size = ftell(fp);

	char* buffer = new char[size];
	::memset(buffer, 0, size);

	fseek(fp, 0, SEEK_SET);
	fread(buffer, size, 1, fp);
	fclose(fp);

	char* dataPtr = (char*)buffer;
	const int32 fileSize = (*(int32*)dataPtr);		dataPtr += 4;
	const int32 zSize = (*(int32*)dataPtr);			dataPtr += 4;
	const int32 xSize = (*(int32*)dataPtr);			dataPtr += 4;
	
	MapDataManager::GetInstnace()->SetSize(zSize, xSize);
	MapDataManager::GetInstnace()->MapData().resize(zSize + 1, std::vector<int>(xSize + 1, 0));

	for (int32 z = 0; z <= zSize; z++)
	{
		for (int32 x = 0; x <= xSize; x++)
		{
			int32 data = (*(int32*)dataPtr); dataPtr += 4;
			MapDataManager::GetInstnace()->MapData()[z][x] = data;
		}
	}

	std::cout << "MapData Parse End" << std::endl;

	std::cout << "SpawnZone Init Start" << std::endl;
	const int32 zrange = zSize / 32;
	const int32 xrange = xSize / 32;
	Map::GetInstance()->SpawnZones().resize(zrange);

	for (int32 z = 0; z < zrange; z++)
		Map::GetInstance()->SpawnZones()[z].resize(xrange);

	for (int32 z = 0; z < zrange; z++)
	{
		for (int32 x = 0; x < xrange; x++)
		{
			int32 sz = z * 32;
			int32 sx = x * 32;

			int32 ez = sz + 32;
			int32 ex = sx + 32;

			BoundBox bound{ ex,ez,sx,sz, };
			Map::GetInstance()->SpawnZones()[z][x] = new SpawnZone(1, bound, MonsterType::Bear);
		}
	}
	

	Map::GetInstance()->SetSMaxSize(xrange, zrange);
	std::cout << "SpawnZone Init End" << std::endl;
}

void Update() 
{
	Map::GetInstance()->Update();
}

int main()
{
	Init();

	const char* ip = "127.0.0.1";
	ServerService service(ip, 7777, GameSession::MakeGameSession);

	service.IOCPRun();
	service.ThreadStart();
	
	while (true) 
	{
		Update();
		Sleep(1);
	}

	return 0;
}