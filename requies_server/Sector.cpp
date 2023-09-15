#include "pch.h"
#include "Sector.h"
#include "GameSession.h"
#include "Player.h"
#include "BufferWriter.h"

Sector::Sector()
{
	InitializeCriticalSection(&_cs);
}

Sector::~Sector()
{
	DeleteCriticalSection(&_cs);
}

void Sector::Set(GameSession* session)
{
	EnterCriticalSection(&_cs);

	_sessions.insert(session);

	LeaveCriticalSection(&_cs);
}

void Sector::Reset(GameSession* session)
{
	EnterCriticalSection(&_cs);

	_sessions.erase(session);

	LeaveCriticalSection(&_cs);
}

void Sector::BroadCast(GameSession* session, BYTE* sendBuffer, int32 sendSize)
{
	EnterCriticalSection(&_cs);

	for (auto s : _sessions)
		s->Send(sendBuffer, sendSize);

	LeaveCriticalSection(&_cs);
}

void Sector::SendPlayerList(GameSession* session)
{
	Player* player = session->GetPlayer();

	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(session->GetSessionID());
	bw.Write((uint16)player->GetState());
	bw.Write((uint16)player->GetDir());
	bw.Write((uint16)player->GetMouseDir());
	bw.Write(player->GetPos());
	bw.Write(player->GetCameraLocalRotation());

	pktHeader->_type = PacketProtocol::S2C_PLAYERNEW;
	pktHeader->_pktSize = bw.GetWriterSize();

	EnterCriticalSection(&_cs);

	for (auto s : _sessions)
	{
		if (s->GetSessionID() == session->GetSessionID())
			continue;
		s->Send(sendBuffer, pktHeader->_pktSize);
	}

	int32 playerCount = _sessions.size();

	if (playerCount > 0)
	{
		int32 packetHeaderSize = 4;
		int32 playerCntSize = 4;
		int32 dataSize = 38;
		int32 playerCnt = _sessions.size();

		const int32 allocSize = packetHeaderSize + playerCntSize + (playerCnt * dataSize);
		BYTE* sendBuffer2 = new BYTE[allocSize];

		BufferWriter bw(sendBuffer2);
		PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();
		pktHeader->_type = PacketProtocol::S2C_PLAYERLIST;
		pktHeader->_pktSize = packetHeaderSize + playerCntSize + (playerCnt * dataSize);

		bw.Write(playerCnt);
		for (auto s : _sessions)
		{
			Player* p = s->GetPlayer();
			bw.Write(s->GetSessionID());		//4
			bw.Write((uint16)p->GetState());    // 2
			bw.Write((uint16)p->GetDir());       // 2
			bw.Write((uint16)p->GetMouseDir());   // 2
			bw.Write(p->GetPos()); // 12
			bw.Write(p->GetCameraLocalRotation()); //16
		}

		session->Send(sendBuffer2, pktHeader->_pktSize);

		if (sendBuffer2)
		{
			delete[] sendBuffer2;
			sendBuffer2 = nullptr;
		}
	}

	LeaveCriticalSection(&_cs);
}

void Sector::SendRemoveList(GameSession* session)
{
	Player* player = session->GetPlayer();

	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(session->GetSessionID());
	bw.Write((uint16)player->GetState());
	bw.Write((uint16)player->GetDir());
	bw.Write((uint16)player->GetMouseDir());
	bw.Write(player->GetPos());
	bw.Write(player->GetCameraLocalRotation());

	pktHeader->_type = PacketProtocol::S2C_PLAYEROUT;
	pktHeader->_pktSize = bw.GetWriterSize();

	EnterCriticalSection(&_cs);

	int32 playerCount = _sessions.size();

	if (playerCount > 0)
	{
		int32 packetHeaderSize = 4;
		int32 playerCntSize = 4;
		int32 dataSize = 4;
		int32 playerCnt = _sessions.size();

		const int32 allocSize = packetHeaderSize + playerCntSize + (playerCnt * dataSize);
		BYTE* sendBuffer2 = new BYTE[allocSize];

		BufferWriter bw(sendBuffer2);
		PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();
		pktHeader->_type = PacketProtocol::S2C_PLAYERREMOVELIST;
		pktHeader->_pktSize = packetHeaderSize + playerCntSize + (playerCnt * dataSize);

		bw.Write(playerCnt);
		for (auto s : _sessions)
		{
			Player* p = s->GetPlayer();
			bw.Write(s->GetSessionID());
		}

		session->Send(sendBuffer2, pktHeader->_pktSize);

		if (sendBuffer2)
		{
			delete[] sendBuffer2;
			sendBuffer2 = nullptr;
		}

	}

	for (auto s : _sessions)
	{
		if (s->GetSessionID() == session->GetSessionID())
			continue;

		s->Send(sendBuffer, pktHeader->_pktSize);
	}

	LeaveCriticalSection(&_cs);
}