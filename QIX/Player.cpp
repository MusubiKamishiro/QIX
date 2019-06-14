#include "Player.h"
#include <DxLib.h>
#include "Peripheral.h"
#include "Game.h"
#include <algorithm>
#include <queue>

#include <random>
#include <string>


Player::Player()
{
	// 画像をランダムで決定
	/*std::random_device rd;
	std::mt19937 mt(rd());

	int num;
	
	std::string s = "img/" + std::to_string((mt() % 8) + 1) + ".png";*/
	//img = DxLib::LoadGraph(s.c_str());
	img = DxLib::LoadGraph("img/bg1.png");		// ゲームを進めると現れる差分(ご褒美)画像
	simg = DxLib::LoadGraph("img/bg2.png");		// 最初の画像

	auto screenSize = Game::Instance().GetScreenSize();
	auto fieldSize = Game::Instance().GetFieldSize();

	up = fieldSize.dotA.y;
	right = fieldSize.dotB.x;
	left = fieldSize.dotA.x;
	down = fieldSize.dotB.y;

	BG = DxLib::MakeScreen((right - left), (down - up));


	dotLegion.resize(0);
	lineLegion.resize(0);
	outLineLegion.resize(0);
	boxLegion.resize(0);

	// 外線
	outLineLegion.push_back(Line(0, up, screenSize.x, up));
	outLineLegion.push_back(Line(0, down, screenSize.x, down));
	outLineLegion.push_back(Line(left, up, left, down));
	outLineLegion.push_back(Line(right, up, right, down));

	// このとき外線はすでに色を変えておく
	for (int y = fieldSize.dotA.y; y <= fieldSize.dotB.y; ++y)
	{
		for (int x = fieldSize.dotA.x; x <= fieldSize.dotB.x; ++x)
		{
			if ((x == fieldSize.dotA.x) || (x == fieldSize.dotB.x) || (y == fieldSize.dotA.y) || (y == fieldSize.dotB.y))
			{
				seeds.at((x - fieldSize.dotA.x) + (y - fieldSize.dotA.y) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)) = { Vector2(x, y), 0xff0000 };
			}
			else
			{
				seeds.at((x - fieldSize.dotA.x) + (y - fieldSize.dotA.y) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)) = { Vector2(x, y), 0x000000 };
			}
		}
	}

	vel = Vector2(0, 0);
	moveVel = 5;
	moveDir = DIR_DOWN;
	oldMoveDir = moveDir;
	startPos = Vector2(150, down);
	pos = startPos;
	
	updater = &Player::MoveOn;
}

Player::~Player()
{
}

void Player::Update(const Peripheral &p)
{
	// 塗りつぶしで出てくる画像
	DxLib::SetDrawScreen(BG);
	DxLib::DrawExtendGraph(0, 0, (right - left), (down - up), img, true);
	// 最初に見える画像
	DxLib::SetDrawScreen(DX_SCREEN_BACK);		// 描画先を戻す
	DxLib::SetDrawBright(150, 150, 150);
	DxLib::DrawExtendGraph(left, up, (right), (down), simg, true);
	DxLib::SetDrawBright(255, 255, 255);

	// 移動方向が決まる
	(this->*updater)(p);

	if ((p.IsTrigger(PAD_INPUT_1)) && (updater == &Player::MoveOn))
	{
		startPos = pos;
		dotLegion.push_back(startPos);
		updater = &Player::DrawLineMove;
	}

	// 前フレームの移動方向と違ったら頂点の追加
	if (updater == &Player::DrawLineMove)
	{
		if (oldMoveDir != moveDir)
		{
			if (dotLegion.size())
			{
				if (dotLegion.back() != pos)
				{
					AddDotAndLine(pos);
				}
			}
		}
	}
	oldMoveDir = moveDir;
	Vector2 oldpos = pos;
	pos += vel;

	NotOutOfRange();

	if(pos != startPos)
	{
		for (int a = 0; a < outLineLegion.size(); a++)
		{
			auto b = outLineLegion.at(a);
			if (b.HitDot(pos))
			{
				startPos = pos;

				if (dotLegion.size())
				{
					AddDotAndLine(pos);
					AddPolygon();
					updater = &Player::MoveOn;
				}
			}
		}
	}

	PolygonDraw();
	if (updater == &Player::DrawLineMove)
	{
		LineDraw();
	}
	OutLineDraw();
	
	
	for (auto& d : seeds)
	{
		if ((d.color != 0x000000) && (d.color != 0xff0000))
		{
			DxLib::DrawRectGraph(d.pos.x, d.pos.y, (d.pos.x - left), (d.pos.y - up), 1, 1, BG, true, false);
		}
	}

	Draw(pos);


#ifdef _DEBUG
	DebugDraw();

#endif // _DEBUG
}

Vector2 Player::GetPos() const
{
	return pos;
}


void Player::Move(const Peripheral & p)
{
	// ボタンを押したら移動(今回は斜め移動禁止)
	if (p.IsPressing(PAD_INPUT_UP))
	{
		moveDir = DIR_UP;
		vel = Vector2(0, -moveVel);
	}
	else if (p.IsPressing(PAD_INPUT_DOWN))
	{
		moveDir = DIR_DOWN;
		vel = Vector2(0, moveVel);
	}
	else if (p.IsPressing(PAD_INPUT_RIGHT))
	{
		moveDir = DIR_RIGHT;
		vel = Vector2(moveVel, 0);
	}
	else if (p.IsPressing(PAD_INPUT_LEFT))
	{
		moveDir = DIR_LEFT;
		vel = Vector2(-moveVel, 0);
	}
	else
	{
		vel = Vector2(0, 0);
	}
}

void Player::MoveOn(const Peripheral &p)
{
	Move(p);

	// 行き先が線上でなければ移動させない
	auto npos = pos;
	npos += vel;
	auto v = vel;

	for (auto& line : outLineLegion)
	{
		if (line.HitDot(pos))
		{
			if (!line.HitDot(npos))
			{
				vel = Vector2(0, 0);
			}
			else
			{
				vel = v;
				break;
			}
		}
	}

}

void Player::DrawLineMove(const Peripheral &p)
{
	Move(p);

	auto npos = pos;
	npos += vel;

	for (auto line : lineLegion)
	{
		if (line.HitDot(npos))
		{
			vel = Vector2(0, 0);
		}
	}

	Line line = Line(dotLegion.back(), pos);
	if (line.HitDot(npos))
	{
		vel = Vector2(0, 0);
	}
}

void Player::Die(const Peripheral &p)
{
}


void Player::Draw(Vector2& pos)
{
	DxLib::DrawCircle(pos.x, pos.y, 2, 0xffffff, true);
	DxLib::DrawLine(pos.x - 7, pos.y, pos.x, pos.y - 7, 0xffffff, 1);
	DxLib::DrawLine(pos.x, pos.y - 7, pos.x + 7, pos.y, 0xffffff, 1);
	DxLib::DrawLine(pos.x + 7, pos.y, pos.x, pos.y + 7, 0xffffff, 1);
	DxLib::DrawLine(pos.x, pos.y + 7, pos.x - 7, pos.y, 0xffffff, 1);
}

void Player::AddDotAndLine(Vector2& pos)
{
	// 点の追加
	Vector2 dot = dotLegion.back();
	dotLegion.push_back(pos);

	Line line = Line(dot, pos);
	lineLegion.push_back(line);
}

void Player::AddPolygon()
{
	auto fieldSize = Game::Instance().GetFieldSize();
	// 外線に変更
	for (int i = 0; i < (lineLegion.size()); i++)
	{
		Line l = lineLegion.at(i);
		outLineLegion.push_back(l);

		for (int y = l.dotA.y; y <= l.dotB.y; ++y)
		{
			for (int x = l.dotA.x; x <= l.dotB.x; ++x)
			{
				seeds.at((x - fieldSize.dotA.x) + (y - fieldSize.dotA.y) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)) = { Vector2(x, y), 0xff0000 };
			}
		}
	}


	std::random_device rd;
	std::mt19937 mt(rd());

	// シードを探す
	Seed seed;
	// シードの開始位置を決める
	bool flag = true;
	while (flag)
	{
		int num = mt() % seeds.size();
		seed = seeds.at(num);
		if (seed.color == 0x000000)
		{
			flag = false;
		}
	}
	std::queue<Seed> seedQueue;
	seedQueue.push(seed);

	while (seedQueue.size())
	{
		// シードから左右の限界を見つける
		int rx = right, lx = left;
		for (Vector2 rseed = Vector2((seed.pos.x + 1), seed.pos.y); rseed.x <= right; ++rseed.x)
		{
			if (seeds.at((rseed.x - fieldSize.dotA.x) + (rseed.y - fieldSize.dotA.y) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)).color != 0x000000)
			{
				rx = rseed.x;
				break;
			}
		}
		for (Vector2 lseed = Vector2((seed.pos.x - 1), seed.pos.y); lseed.x >= left; --lseed.x)
		{
			if (seeds.at((lseed.x - fieldSize.dotA.x) + (lseed.y - fieldSize.dotA.y) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)).color != 0x000000)
			{
				lx = lseed.x;
				break;
			}
		}

		// 上で見つけた端から端まで塗りつぶす
		for (int i = (lx + 1); i < rx; ++i)
		{
			seeds.at((i - fieldSize.dotA.x) + (seed.pos.y - fieldSize.dotA.y) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)).color = 0x00ff00;
		}

		// 上の1つ上の左右までみて塗れる右側をシード候補に入れる
		for (int i = lx + 1; i <= rx; ++i)
		{
			if (seeds.at((i - fieldSize.dotA.x) + (seed.pos.y - fieldSize.dotA.y - 1) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)).color != 0x000000)
			{
				if (seeds.at((i - fieldSize.dotA.x) + (seed.pos.y - fieldSize.dotA.y - 1) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)).color == 0xff0000)
				{
					seedQueue.push(seeds.at((i - fieldSize.dotA.x - 1) + (seed.pos.y - fieldSize.dotA.y - 1) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)));
				}
			}

			if (i == rx)
			{
				if (seeds.at((i - fieldSize.dotA.x) + (seed.pos.y - fieldSize.dotA.y - 1) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)).color == 0x000000)
				{
					seedQueue.push(seeds.at((i - fieldSize.dotA.x - 1) + (seed.pos.y - fieldSize.dotA.y - 1) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)));
				}
			}
		}
		// 上の1つ下の左右までみて塗れる右側をシード候補に入れる
		for (int i = lx + 1; i <= rx; ++i)
		{
			if (seeds.at((i - fieldSize.dotA.x) + (seed.pos.y - fieldSize.dotA.y + 1) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)).color != 0x000000)
			{
				if (seeds.at((i - fieldSize.dotA.x) + (seed.pos.y - fieldSize.dotA.y + 1) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)).color == 0xff0000)
				{
					seedQueue.push(seeds.at((i - fieldSize.dotA.x - 1) + (seed.pos.y - fieldSize.dotA.y + 1) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)));
				}
			}

			if (i == rx)
			{
				if (seeds.at((i - fieldSize.dotA.x) + (seed.pos.y - fieldSize.dotA.y + 1) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)).color == 0x000000)
				{
					seedQueue.push(seeds.at((i - fieldSize.dotA.x - 1) + (seed.pos.y - fieldSize.dotA.y + 1) * (fieldSize.dotB.x - fieldSize.dotA.x + 1)));
				}
			}
		}

		flag = true;
		while (flag && seedQueue.size())
		{
			// シード候補から1つ取り出してシードに入れる
			seedQueue.pop();
			if (!seedQueue.size())
			{
				break;
			}
			seed = seedQueue.front();
			
			// シードが有効なら繰り返す
			if (seed.color == 0x000000)
			{
				flag = false;
			}
		}
	}

	lineLegion.erase(lineLegion.begin(), lineLegion.end());
	dotLegion.erase(dotLegion.begin(), dotLegion.end());
	lineLegion.resize(0);
	dotLegion.resize(0);
}


void Player::LineDraw()
{
	// 線の集合を元に描画
	for (int a = 0; a < lineLegion.size(); a++)
	{
		auto line = lineLegion.data() + a;
		DxLib::DrawLine(line->dotA.x, line->dotA.y, line->dotB.x, line->dotB.y, 0x00ff00);
	}
	if (dotLegion.size())
	{
		DxLib::DrawLine(dotLegion.back().x, dotLegion.back().y, pos.x, pos.y, 0x00ff00);
	}
}

void Player::OutLineDraw()
{
	// 線の集合を元に描画
	for (int a = 0; a < outLineLegion.size(); a++)
	{
		auto line = outLineLegion.data() + a;
		DxLib::DrawLine(line->dotA.x, line->dotA.y, line->dotB.x, line->dotB.y, 0x00ff00);
	}
}

void Player::PolygonDraw()
{
	for(auto box : boxLegion)
	{
		//DxLib::DrawBox(box.dotA.x, box.dotA.y, box.dotB.x, box.dotB.y, 0xffff00, true);
		DxLib::DrawRectGraph(box.dotA.x, box.dotA.y, (box.dotA.x - left), (box.dotA.y - up), (box.dotB.x - box.dotA.x), (box.dotB.y - box.dotA.y), BG, true, false);
	}
}

void Player::DebugDraw()
{
	std::string s = "boxの数：";
	s += std::to_string(boxLegion.size());
	DxLib::DrawString(550, 20, s.c_str(), 0x00ffff);

	s = "未確定線の数：";
	s += std::to_string(lineLegion.size());
	DxLib::DrawString(550, 40, s.c_str(), 0x00ffff);

	s = "確定線の数：";
	s += std::to_string(outLineLegion.size());
	DxLib::DrawString(550, 60, s.c_str(), 0x00ffff);

	for (auto& a : seeds)
	{
		if (a.color == 0xffffff)
		{
			DxLib::DrawString(a.pos.x, a.pos.y, "0", 0xff0000);
		}
	}
}

bool Player::NotOutOfRange()
{
	bool flag = false;

	// 範囲外にはいかせないぜ
	if (pos.x <= left)
	{
		pos.x = left;
		flag = true;
	}
	else if (pos.x >= right)
	{
		pos.x = right;
		flag = true;;
	}

	if (pos.y <= up)
	{
		pos.y = up;
		flag = true;
	}
	else if (pos.y >= down)
	{
		pos.y = down;
		flag = true;
	}

	return flag;
}
