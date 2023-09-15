#include "pch.h"
#include "RecvBuffer.h"

bool RecvBuffer::Read(int32 numOfBytes, BYTE** dataPtr, int32& dataLen)
{
	if (_writePos + numOfBytes > RECV_BUFFER_CHUNK * 4)
	{
		wprintf(L"recvBuffer Left Size %d", GetFreeSize());

		int32 dataSize = GetDataSize() + numOfBytes;
		::memcpy(_buffer.data(), &_buffer[_readPos], dataSize);
		_writePos = GetDataSize();
		_readPos = 0;
	}

	_writePos += numOfBytes;

	if (GetDataSize() < sizeof(PacketHeader))
		return false;

	PacketHeader* header = reinterpret_cast<PacketHeader*>(GetReadPos());

	if (GetDataSize() < header->_pktSize)
		return false;

	*dataPtr = GetReadPos();
	dataLen = header->_pktSize;

	return true;
}

void RecvBuffer::AddRecvPos(int32 dataLen)
{
	_readPos += dataLen;

	if (_readPos == _writePos)
	{
		_readPos = 0;
		_writePos = 0;
	}
}
