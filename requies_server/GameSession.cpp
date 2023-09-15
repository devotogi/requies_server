#include "pch.h"
#include "GameSession.h"
#include "Player.h"
#include "SessionManager.h"
#include "BufferWriter.h"
#include "PacketHandler.h"
#include "MapManager.h"

int	 GetRandom0to7()
{
	thread_local std::mt19937 generator(std::random_device{}());

	std::uniform_int_distribution<int> distribution(0, 7);

	return distribution(generator);
}

GameSession::GameSession(const SOCKET& socket, const SOCKADDR_IN& sock) : Session(socket, sock)
{
}

GameSession::~GameSession()
{
	if (_player)
		delete _player;
}

void GameSession::OnRecv(Session* session, BYTE* dataPtr, int32 dataLen)
{
	PacketHandler::HandlePacket(static_cast<GameSession*>(session), dataPtr, dataLen);
}

void GameSession::OnDisconnect()
{
	SessionManager::GetInstance()->PopSession(this);
}

void GameSession::OnConnect()
{
	SessionManager::GetInstance()->GetSessionId(_sessionId);

	int32 randomPosx = GetRandom0to7();
	int32 randomPosz = GetRandom0to7();

	// Vector3 startPos = MapManager::GetInstance()->GetStartPos(randomPosx, randomPosz);
	Vector3 startPos = { 70,0,70 };
	_player = new Player(this, _sessionId, startPos);
	SessionManager::GetInstance()->AddSession(this);

	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	bw.Write(_sessionId);
	bw.Write((uint16)_player->GetState());
	bw.Write((uint16)_player->GetDir());
	bw.Write((uint16)_player->GetMouseDir());
	bw.Write(_player->GetPos());
	bw.Write(_player->GetCameraLocalRotation());

	pktHeader->_type = PacketProtocol::S2C_PLAYERINIT;
	pktHeader->_pktSize = bw.GetWriterSize();

	Send(sendBuffer, pktHeader->_pktSize);
	MapManager::GetInstance()->Set(this);
}
