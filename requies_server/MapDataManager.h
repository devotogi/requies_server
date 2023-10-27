#pragma once
class MapDataManager
{
private:
	int32 _zSize;
	int32 _xSize;
	std::vector<std::vector<int32>> _mapData;
	std::stack<int32> _monsterIds;
	long long _monsterId = 0;

public:
	static MapDataManager* GetInstnace() 
	{
		static MapDataManager _mapDataManager;
		return &_mapDataManager;
	}

	std::vector<std::vector<int32>>& MapData() 
	{
		return _mapData;
	}

	void SetSize(int32 zSize, int32 xSize) 
	{
		_zSize = zSize; 
		_xSize = xSize;
	}

	bool CanGo(int32 z, int32 x) 
	{
		if (z > _zSize)
			return false;

		if (x > _xSize)
			return false;

		if (_mapData[z][x] == 1)
			return false;

		return true;
	}

	int32 PopMonsterId() 
	{
		int32 ret;

		if (_monsterIds.empty())
		{
			ret = ++_monsterId;
		}
		else
		{
			ret = _monsterIds.top();
			_monsterIds.pop();
		}

		return ret;
	}

	int32 PushMonsterId(int32 id) 
	{
		_monsterIds.push(id);
	}
};
