#include "pch.h"
#include "MapDataManager.h"

void MapDataManager::FindPath(const Vector3& dest, const Vector3& start, std::vector<Pos>& path)
{
	Pos startPos = { static_cast<int32>(start.x), static_cast<int32>(start.z) };
	Pos endPos = { static_cast<int32>(dest.x), static_cast<int32>(dest.z) };

	if (CanGo(endPos) == false)
		return;

	Pos front[8] =
	{
		{0, 1}, // ��
		{1, 0}, // ������
		{0, -1}, // �Ʒ���
		{-1, 0}, //����
		{1, 1}, // �� ������
		{-1, 1}, // �� ����
		{1, -1}, // �Ʒ� ������
		{-1,-1} // �Ʒ� ����
	};

	int32 cost[8] =
	{
		10,
		10,
		10,
		10,
		14,
		14,
		14,
		14
	};

	int32 dirCount = 8;

	std::vector<std::vector<int32>> best(_zSize + 1, std::vector<int32>(_xSize + 1, INT32_MAX));

	std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> pq;

	std::map<Pos, Pos> parent;

	{
		int32 g = 0;
		int32 h = 10 * (abs(endPos.z - endPos.z) + abs(startPos.z - startPos.x));
		pq.push({ g + h, g, startPos });
		best[startPos.z][startPos.x] = g + h;
		parent[startPos] = startPos;
	}

	while (pq.empty() == false)
	{
		PQNode node = pq.top();
		pq.pop();

		Pos pos = node.pos;

		if (best[pos.z][pos.x] < node.f)
			continue;

		if (node.pos == endPos)
			break;

		for (int dir = 0; dir < dirCount; dir++)
		{
			Pos nextPos = pos + front[dir];

			if (CanGo(nextPos) == false) continue;

			int nextG = node.g + cost[dir];
			int nextH = 10 * (abs(endPos.z - nextPos.z) + abs(endPos.x - nextPos.x));

			if (best[nextPos.z][nextPos.x] <= nextG + nextH) continue;

			pq.push({ nextG + nextH, nextG, nextPos });
			best[nextPos.z][nextPos.x] = nextG + nextH;
			parent[nextPos] = pos;
		}
	}

	Pos pos = endPos;

	while (true)
	{
		path.push_back(pos);
		pos = parent[pos];
		if (pos == startPos)
			break;
	}

	std::reverse(path.begin(), path.end());
}
