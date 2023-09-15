#pragma once
class BufferReader
{
private:
	BYTE* _buffer;
	int32 _readPos = 0;
public:
	BufferReader(BYTE* buffer);
	~BufferReader();

	template<typename T>
	void Read(T& data);

	void ReadWString(WCHAR* str, int32 strSize);

	int32 GetReadSize() { return _readPos; }
};

template<typename T>
inline void BufferReader::Read(T& data)
{
	int typeSize = sizeof(T);
	data = *(T*)&_buffer[_readPos];
	_readPos += typeSize;
}
