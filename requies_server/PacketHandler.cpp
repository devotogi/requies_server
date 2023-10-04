#include "pch.h"
#include "PacketHandler.h"
#include "Player.h"
#include "GameSession.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Map.h"
#include "SessionManager.h"
void PacketHandler::HandlePacket(GameSession* session, BYTE* packet, int32 packetSize)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(packet);
	BYTE* dataPtr = packet + sizeof(PacketHeader);
	int32 dataSize = packetSize - sizeof(PacketHeader);

	switch (header->_type)
	{
	case PacketProtocol::C2S_PLAYERSYNC: // 플레이어 동기화
		HandlePacket_C2S_PLAYERSYNC(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_MAPSYNC: // 플레이어가 1칸마다 좌표 동기화 
		HandlePacket_C2S_MAPSYNC(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_LATENCY: // 모니터 레이턴시
		HandlePacket_C2S_LATENCY(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_PLAYERATTACK:
		HandlePacket_C2S_PLAYERATTACK(session, dataPtr, dataSize);
		break;
	}
}

void PacketHandler::HandlePacket_C2S_PLAYERSYNC(GameSession* session, BYTE* packet, int32 packetSize)
{
	Player* player = session->GetPlayer();
	BufferReader br(packet);

	int32 playerId;
	State state;
	Dir dir;
	Dir mouseDir;
	Vector3 vector3;
	Quaternion quaternion;
	Vector3 target;
	MoveType moveType;

	br.Read(playerId);
	br.Read(state);
	br.Read(dir);
	br.Read(mouseDir);
	br.Read(vector3);
	br.Read(quaternion);
	br.Read(target);
	br.Read(moveType);

	player->PlayerSync(vector3, state, dir, mouseDir, quaternion, target, moveType);

	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(session->GetSessionID());
	bw.Write((uint16)player->GetState());
	bw.Write((uint16)player->GetDir());
	bw.Write((uint16)player->GetMouseDir());
	bw.Write(player->GetPos());
	bw.Write(player->GetCameraLocalRotation());
	bw.Write(target);
	bw.Write(moveType);

	pktHeader->_type = PacketProtocol::S2C_PLAYERSYNC;
	pktHeader->_pktSize = bw.GetWriterSize();

	Map::GetInstance()->BroadCast(session, sendBuffer, bw.GetWriterSize());
}

void PacketHandler::HandlePacket_C2S_LATENCY(GameSession* session, BYTE* packet, int32 packetSize)
{
	BufferReader br(packet);
	int32 lastTick;

	br.Read(lastTick);

	BYTE sendBuffer[20];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(lastTick);
	pktHeader->_type = PacketProtocol::S2C_LATENCY;
	pktHeader->_pktSize = bw.GetWriterSize();

	session->Send(sendBuffer, bw.GetWriterSize());
}

void PacketHandler::HandlePacket_C2S_MAPSYNC(GameSession* session, BYTE* packet, int32 packetSize)
{
	Player* player = session->GetPlayer();
	Vector3 prevPos = player->GetPrevPos();
	BufferReader br(packet);

	int32 playerId;
	State state;
	Dir dir;
	Dir mouseDir;
	Vector3 vector3;
	Quaternion quaternion;
	Vector3 target;
	MoveType moveType;

	br.Read(playerId);
	br.Read(state);
	br.Read(dir);
	br.Read(mouseDir);
	br.Read(vector3);
	br.Read(quaternion);
	br.Read(target);
	br.Read(moveType);

	player->PlayerSync(vector3, state, dir, mouseDir, quaternion, target, moveType);
	Map::GetInstance()->MapSync(session, prevPos, vector3);
	player->SetPrevPos(vector3);
}

void PacketHandler::HandlePacket_C2S_PLAYERATTACK(GameSession* session, BYTE* packet, int32 packetSize)
{
	int32 otherPlayer;
	BufferReader br(packet);
	br.Read(otherPlayer);
	GameSession* AttackedSession = SessionManager::GetInstance()->GetSession(otherPlayer);

	// TODO 좌표 계산 

	Player* AttackedPlayer = AttackedSession->GetPlayer();
	AttackedPlayer->Attacked();
}