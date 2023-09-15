#include "pch.h"
#include "PacketHandler.h"
#include "Player.h"
#include "GameSession.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "MapManager.h"

void PacketHandler::HandlePacket(GameSession* session, BYTE* packet, int32 packetSize)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(packet);
	BYTE* dataPtr = packet + sizeof(PacketHeader);
	int32 dataSize = packetSize - sizeof(PacketHeader);

	switch (header->_type)
	{
	case PacketProtocol::C2S_PLAYERSYNC: // �÷��̾� ����ȭ
		HandlePacket_C2S_PLAYERSYNC(session, dataPtr, dataSize);
		break;

	case PacketProtocol::C2S_MAPSYNC: // �÷��̾ 1ĭ���� ��ǥ ����ȭ 
		HandlePacket_C2S_MAPSYNC(session, dataPtr, dataSize);
		break;


	case PacketProtocol::C2S_LATENCY: // ����� �����Ͻ�
		HandlePacket_C2S_LATENCY(session, dataPtr, dataSize);
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

	br.Read(playerId);
	br.Read(state);
	br.Read(dir);
	br.Read(mouseDir);
	br.Read(vector3);
	br.Read(quaternion);

	player->PlayerSync(vector3, state, dir, mouseDir, quaternion);

	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(session->GetSessionID());
	bw.Write((uint16)player->GetState());
	bw.Write((uint16)player->GetDir());
	bw.Write((uint16)player->GetMouseDir());
	bw.Write(player->GetPos());
	bw.Write(player->GetCameraLocalRotation());

	pktHeader->_type = PacketProtocol::S2C_PLAYERSYNC;
	pktHeader->_pktSize = bw.GetWriterSize();

	MapManager::GetInstance()->BroadCast(session, sendBuffer, bw.GetWriterSize());
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

	br.Read(playerId);
	br.Read(state);
	br.Read(dir);
	br.Read(mouseDir);
	br.Read(vector3);
	br.Read(quaternion);

	player->PlayerSync(vector3, state, dir, mouseDir, quaternion);

	MapManager::GetInstance()->Sync(session, prevPos, vector3);
	player->SetPrevPos(vector3);
}