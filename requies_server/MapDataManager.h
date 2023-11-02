#pragma once
#include "GameObject.h"

class MapDataManager
{
private:
	int32 _zSize;
	int32 _xSize;
	std::vector<std::vector<int32>> _mapData;
	std::vector<std::vector<std::set<GameObject*>>> _mapDataGameObject;
	std::stack<int32> _monsterIds;
	long long _monsterId = 0;

	CRITICAL_SECTION _cs;
	CRITICAL_SECTION _mapDataGameObjectCs;

public:
	static MapDataManager* GetInstnace() 
	{
		static MapDataManager _mapDataManager;
		return &_mapDataManager;
	}

	MapDataManager() 
	{
		InitializeCriticalSection(&_cs);
		InitializeCriticalSection(&_mapDataGameObjectCs);
	}

	~MapDataManager() 
	{
		DeleteCriticalSection(&_cs);
		DeleteCriticalSection(&_mapDataGameObjectCs);
	}

	std::vector<std::vector<int32>>& MapData() 
	{
		return _mapData;
	}

	std::vector<std::vector<std::set<GameObject*>>>& MapDataGameObject()
	{
		return _mapDataGameObject;
	}

	void ApplyMapGameObject(GameObject* object, bool add)
	{
		EnterCriticalSection(&_mapDataGameObjectCs);
		
		Vector3 playerPos = object->GetPos();
		int32 x = static_cast<int32>(playerPos.x);
		int32 z = static_cast<int32>(playerPos.z);

		if (add)
			_mapDataGameObject[z][x].insert(object);

		else
			_mapDataGameObject[z][x].erase(object);

		LeaveCriticalSection(&_mapDataGameObjectCs);
	}

	void ApplyMapGameObject(GameObject* object, Vector3 pos, bool add) 
	{
		EnterCriticalSection(&_mapDataGameObjectCs);

		int32 x = static_cast<int32>(pos.x);
		int32 z = static_cast<int32>(pos.z);

		if (add)
			_mapDataGameObject[z][x].insert(object);

		else
			_mapDataGameObject[z][x].erase(object);

		LeaveCriticalSection(&_mapDataGameObjectCs);
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

		if (z < 0)
			return false;

		if (x < 0)
			return false;

		if (_mapData[z][x] == 1)
			return false;

		return true;
	}

	bool CanGo(const Pos& pos)
	{
		if (pos.z > _zSize)
			return false;

		if (pos.x > _xSize)
			return false;

		if (pos.z < 0)
			return false;

		if (pos.x < 0)
			return false;

		if (_mapData[pos.z][pos.x] == 1)
			return false;

		return true;
	}

	int32 PopMonsterId() 
	{
		int32 ret;
		EnterCriticalSection(&_cs);
		if (_monsterIds.empty())
		{
			ret = ++_monsterId;
		}
		else
		{
			ret = _monsterIds.top();
			_monsterIds.pop();
		}
		LeaveCriticalSection(&_cs);
		return ret;
	}

	void PushMonsterId(int32 id) 
	{
		EnterCriticalSection(&_cs);
		_monsterIds.push(id);
		LeaveCriticalSection(&_cs);
	}

	void FindPath(const Vector3& dest, const Vector3& start, std::vector<Pos>& path);
};

