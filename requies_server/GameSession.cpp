#include "pch.h"
#include "GameSession.h"
#include "Player.h"
#include "SessionManager.h"
#include "BufferWriter.h"
#include "PacketHandler.h"
#include "Map.h"

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
	SessionManager::GetInstance()->PopSession(_sessionId);
}

void GameSession::OnConnect()
{
	SessionManager::GetInstance()->GetSessionId(_sessionId);

	int32 randomPosx = GetRandom0to7();
	int32 randomPosz = GetRandom0to7();

	Vector3 startPos = Map::GetInstance()->GetStartPos(randomPosx, randomPosz);
	_player = new Player(this, _sessionId, {74,0,70});
	SessionManager::GetInstance()->AddSession(_sessionId, this);

	BYTE sendBuffer[100];
	BufferWriter bw(sendBuffer);
	PacketHeader* pktHeader = bw.WriteReserve<PacketHeader>();

	int32 sessionId = _sessionId;
	uint16 playerState = (uint16)_player->GetState();
	uint16 playerDir = (uint16)_player->GetDir();
	uint16 playerMouseDir = (uint16)_player->GetMouseDir();
	Vector3 playerPos = _player->GetPos();
	Quaternion playerQuaternion = _player->GetCameraLocalRotation();

	bw.Write(sessionId);
	bw.Write(playerState);
	bw.Write(playerDir);
	bw.Write(playerMouseDir);
	bw.Write(playerPos);
	bw.Write(playerQuaternion);

	pktHeader->_type = PacketProtocol::S2C_PLAYERINIT;
	pktHeader->_pktSize = bw.GetWriterSize();

	Send(sendBuffer, pktHeader->_pktSize);
	Map::GetInstance()->Set(this);
}
