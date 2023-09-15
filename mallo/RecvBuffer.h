#pragma once
#define RECV_BUFFER_CHUNK 4096
#define RECV_BUFFER_COUNT 5
#define RECV_BUFFER_LEN 4096 * RECV_BUFFER_COUNT

class RecvBuffer
{
private:
	std::vector<BYTE>	_buffer;
	int32				_readPos = 0;
	int32				_writePos = 0;
	int32				_bufferSize = RECV_BUFFER_LEN;
public:

	RecvBuffer() { _buffer.resize(RECV_BUFFER_LEN, 0); }

	int32 GetDataSize() { return _writePos - _readPos; }
	int32 GetFreeSize() { return _bufferSize - _writePos; };
	BYTE* GetBuffer() { return _buffer.data(); }
	BYTE* GetReadPos() { return &_buffer[_readPos]; }
	BYTE* GetWritePos() { return &_buffer[_writePos]; }
	bool  Read(int32 numOfBytes, BYTE** dataPtr, int32& dataLen);
	void  AddRecvPos(int32 dataLen);
};

