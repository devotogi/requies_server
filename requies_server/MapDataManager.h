#pragma once
class MapDataManager
{
private:
	int32 _zSize;
	int32 _xSize;
	std::vector<std::vector<int32>> _mapData;

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
};

