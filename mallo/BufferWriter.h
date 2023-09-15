#pragma once
class BufferWriter
{
private:
	BYTE* _buffer;
	int32 _writePos = 0;
public:
	BufferWriter(BYTE* buffer);
	~BufferWriter();

	template<typename T>
	T* WriteReserve();

	template<typename T>
	void Write(T data);

	void WriteWString(WCHAR* str, int32 strSize);

	int32 GetWriterSize() { return _writePos; }

	void Reset(BYTE* _buffer)
	{
		_buffer = _buffer;
		_writePos = 0;
	}

};

template<typename T>
inline T* BufferWriter::WriteReserve()
{
	T* ret = reinterpret_cast<T*>(_buffer);
	_writePos += sizeof(T);
	return ret;
}

template<typename T>
inline void BufferWriter::Write(T data)
{
	int dataSize = sizeof(T);
	::memcpy(&_buffer[_writePos], &data, dataSize);
	_writePos += dataSize;
}
