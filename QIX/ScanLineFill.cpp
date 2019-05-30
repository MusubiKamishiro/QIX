#include "ScanLineFill.h"
#include "Game.h"


void ScanLineFill::AddCell(const Vector2& inv)
{
	int cnt = 0;

	if (cell.size() == 0)
	{
		cell.push_back(inv);
		cellcount.push_back(inv);
	}
	else
	{
		for (auto p : cell)
		{
			if (p == inv)
			{
				cnt++;
				break;
			}
		}

		if (cnt == 0)
		{
			cell.push_back(inv);
			cellcount.push_back(inv);
		}
	}
}

Vector2 ScanLineFill::PopCell()
{
	Vector2 seed = cellcount.at(0);
	cellcount.erase(cellcount.begin());

	return seed;
}

void ScanLineFill::LineToDot(const std::vector<Line>& ll)
{
	linedot.resize(0);

	for (auto line : ll)
	{
		if (line.dir)
		{
			for (int d = line.dotA.x; d < line.dotB.x; d++)
			{
				Vector2 dot = Vector2(d, line.dotA.y);
				linedot.push_back(dot);
			}
		}
		else
		{
			for (int d = line.dotA.y; d < line.dotB.y; d++)
			{
				Vector2 dot = Vector2(line.dotA.x, d);
				linedot.push_back(dot);
			}
		}
		
	}
}

ScanLineFill::ScanLineFill()
{
	cell.resize(0);
	cellcount.resize(0);
	linedot.resize(0);

	auto fieldSize = Game::Instance().GetFieldSize();

	up = fieldSize.dotA.y;
	right = fieldSize.dotB.x;
	left = fieldSize.dotA.x;
	down = fieldSize.dotB.y;
}


ScanLineFill::~ScanLineFill()
{
}

void ScanLineFill::SeedFill(const std::vector<Line>& ll, Vector2& _pos)
{
	// 引いた線を点に変える
	LineToDot(ll);

	// 開始点を加える
	AddCell(_pos);

	// とりあえずつぶす
	while (cellcount.size() > 0)
	{
		Vector2 popup = PopCell();

		for (auto dot : linedot)
		{
			if ((Vector2((popup.x + 1), popup.y) != dot) && ((popup.x + 1) <= right))
			{
				AddCell(Vector2((popup.x + 1), popup.y));
			}
			if ((Vector2((popup.x - 1), popup.y) != dot) && ((popup.x - 1) >= left))
			{
				AddCell(Vector2((popup.x - 1), popup.y));
			}
			if ((Vector2(popup.x, (popup.y + 1)) != dot) && ((popup.y + 1) <= down))
			{
				AddCell(Vector2(popup.x, (popup.y + 1)));
			}
			if ((Vector2(popup.x, (popup.y - 1)) != dot) && ((popup.y - 1) >= up))
			{
				AddCell(Vector2(popup.x, (popup.y - 1)));
			}
		}


	}

	// つぶしたところで四角をつくる

}
