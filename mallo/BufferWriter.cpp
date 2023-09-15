#include "pch.h"
#include "BufferWriter.h"

BufferWriter::BufferWriter(BYTE* buffer) : _buffer(buffer)
{

}

BufferWriter::~BufferWriter()
{
}

void BufferWriter::WriteWString(WCHAR* str, int32 strSize)
{
	WCHAR* startStr = reinterpret_cast<WCHAR*>(&_buffer[_writePos]);
	::memcpy(startStr, str, strSize);
	_writePos += strSize;
}