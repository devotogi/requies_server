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

	case PacketProtocol::C2S_PLAYERCHAT:
		HandlePacket_C2S_PLAYERCHAT(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_PLAYERESPAWN:
		HandlePacket_C2S_PLAYERESPAWN(session, dataPtr, dataSize);
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
	Quaternion localRotation;

	br.Read(playerId);
	br.Read(state);
	br.Read(dir);
	br.Read(mouseDir);
	br.Read(vector3);
	br.Read(quaternion);
	br.Read(target);
	br.Read(moveType);
	br.Read(localRotation);

	player->PlayerSync(vector3, state, dir, mouseDir, quaternion, target, moveType, localRotation);

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
	bw.Write(localRotation);

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
	Quaternion localRotation;

	br.Read(playerId);
	br.Read(state);
	br.Read(dir);
	br.Read(mouseDir);
	br.Read(vector3);
	br.Read(quaternion);
	br.Read(target);
	br.Read(moveType);
	br.Read(localRotation);

	player->PlayerSync(vector3, state, dir, mouseDir, quaternion, target, moveType, localRotation);
	Map::GetInstance()->MapSync(session, prevPos, vector3);
	player->SetPrevPos(vector3);
}

void PacketHandler::HandlePacket_C2S_PLAYERATTACK(GameSession* session, BYTE* packet, int32 packetSize)
{
	int32 otherPlayer;
	int32 damage;

	BufferReader br(packet);
	br.Read(otherPlayer);
	br.Read(damage);

	Player* AttackPlayer = session->GetPlayer();
	GameSession* AttackedSession = SessionManager::GetInstance()->GetSession(otherPlayer);

	if (AttackedSession == nullptr)
		return;

	Player* AttackedPlayer = AttackedSession->GetPlayer();
	
	Vector3 attackedPos = AttackedPlayer->GetPos();
	Vector3 attackerPos = AttackPlayer->GetPos();

	if (attackedPos.x <= attackerPos.x + 2 && attackedPos.x >= attackedPos.x - 2) 
	{
		if (attackedPos.z <= attackerPos.z + 2 && attackedPos.z >= attackedPos.z - 2)
		{
			AttackedPlayer->Attacked(damage);
		}
	}
}

void PacketHandler::HandlePacket_C2S_PLAYERCHAT(GameSession* session, BYTE* packet, int32 packetSize)
{
	int32 chattingMsgSize;
	int32 sessionId = session->GetSessionID();

	BufferReader br(packet);
	WCHAR text[1000] = {0};
	br.Read(chattingMsgSize);
	br.ReadWString(text, chattingMsgSize);
	
	BYTE sendBuffer[1000];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(sessionId);
	bw.Write(chattingMsgSize);
	bw.WriteWString(text, chattingMsgSize);

	pktHeader->_type = PacketProtocol::S2C_PLAYERCHAT;
	pktHeader->_pktSize = bw.GetWriterSize();

	Map::GetInstance()->BroadCast(session, sendBuffer, bw.GetWriterSize());
}

void PacketHandler::HandlePacket_C2S_PLAYERESPAWN(GameSession* session, BYTE* packet, int32 packetSize)
{
	session->GetPlayer()->ReSpawn();
}
