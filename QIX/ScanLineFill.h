#pragma once
#include "Geometry.h"
#include <vector>

class ScanLineFill
{
private:
	Vector2 pos;
	std::vector<Vector2> cell;
	std::vector<Vector2> cellcount;
	std::vector<Vector2> linedot;

	int up;
	int right;
	int left;
	int down;

	void AddCell(const Vector2& inv);		// �ǉ�
	Vector2 PopCell();						// ���o��

	void LineToDot(const std::vector<Line>& ll);		// ������_��

public:
	ScanLineFill();
	~ScanLineFill();

	void SeedFill(const std::vector<Line>& ll, Vector2& _pos);
};

