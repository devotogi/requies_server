#pragma once
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using wchar = wchar_t;

struct PacketHeader
{
	__int16 _type = 0;
	__int16 _pktSize = 0;
};

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;
};

enum State : uint16
{
	IDLE,
	MOVE
};

enum Dir : uint16
{
	NONE = 0,
	UP = 2,
	RIGHT = 4,
	DOWN = 8,
	LEFT = 16,
	UPRIGHT = 6,
	RIGHTDOWN = 12,
	LEFTDOWN = 24,
	LEFTUP = 18,
};

struct Pos
{
public:
	int x;
	int z;

	bool operator==(const Pos& other)
	{
		return x == other.x && z == other.z;
	}

	bool operator!=(const Pos& other)
	{
		return !(*this == other);
	}
};

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3 operator*(float d)
	{
		return { x * d, y * d, z * d };
	}

	Vector3 operator+(Vector3 d)
	{
		return { x + d.x, y + d.y, z + d.z };
	}
};