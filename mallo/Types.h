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

enum MoveType : int32 
{
	KeyBoard,
	Mouse
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

enum PacketProtocol : __int16
{
	C2S_PLAYERINIT,
	S2C_PLAYERINIT,
	C2S_PLAYERSYNC,
	S2C_PLAYERSYNC,
	S2C_PLAYERLIST,
	S2C_PLAYERREMOVELIST,
	S2C_PLAYERENTER,
	S2C_PLAYEROUT,
	C2S_LATENCY,
	S2C_LATENCY,
	C2S_MAPSYNC,
	S2C_MAPSYNC,
	S2C_PLAYERNEW,
	S2C_PLAYERDESTORY,
	C2S_PLAYERATTACK,
	S2C_PLAYERATTACKED,
	C2S_PLAYERCHAT,
	S2C_PLAYERCHAT,
	S2C_PLAYERDETH,
	C2S_PLAYERESPAWN,
	S2C_PLAYERESPAWN,
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

	bool operator<(const Pos& other) const
	{
		if (z != other.z)
			return z < other.z;
		return x < other.x;
	}

	bool operator>(const Pos& other) const
	{
		if (z != other.z)
			return z > other.z;
		return x > other.x;
	}

};

struct PacketHeader
{
	__int16 _type = 0;
	__int16 _pktSize = 0;
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

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;
};
