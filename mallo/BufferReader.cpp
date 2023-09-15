#include "pch.h"
#include "BufferReader.h"

BufferReader::BufferReader(BYTE* buffer) : _buffer(buffer)
{
}

BufferReader::~BufferReader()
{
}

void BufferReader::ReadWString(WCHAR* str, int32 strSize)
{
	::memcpy(str, &_buffer[_readPos], strSize);
	_readPos += strSize;
}
