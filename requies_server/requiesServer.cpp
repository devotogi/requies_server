#include "pch.h"
#include "ServerService.h"
#include "GameSession.h"
#include "ThreadManager.h"
#include "IOCPCore.h"

void Init() 
{
	FILE* fp = NULL;
	fopen_s(&fp,"C:\\Users\\jgkang\\Desktop\\map\\map.dat", "r");
	
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

	std::vector<std::vector<int32>> v (zSize + 1, std::vector<int32>(xSize + 1));

	for (int32 z = 0; z <= zSize; z++)
	{
		for (int32 x = 0; x <= xSize; x++)
		{
			int32 data = (*(int32*)dataPtr);	dataPtr += 4;
			v[z][x] = data;

			if (data != 0)
			{
				int b = 3;
			}
		}
	}

}

unsigned int _stdcall Dispatch(void* Args)
{
	ServerService* service = reinterpret_cast<ServerService*>(Args);
	while (true)
		service->GetIOCPCore()->Dispatch();
}

int main()
{
	Init();

	const char* ip = "127.0.0.1";
	ServerService service(ip, 7777, GameSession::MakeGameSession);

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	int32 threadCount = sysInfo.dwNumberOfProcessors * 2;

	for (int i = 0; i < threadCount; i++)
		ThreadManager::GetInstance()->Launch(Dispatch, &service);

	service.Start();

	return 0;
}