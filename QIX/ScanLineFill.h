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

	void AddCell(const Vector2& inv);		// 追加
	Vector2 PopCell();						// 取り出し

	void LineToDot(const std::vector<Line>& ll);		// 線から点へ

public:
	ScanLineFill();
	~ScanLineFill();

	void SeedFill(const std::vector<Line>& ll, Vector2& _pos);
};

