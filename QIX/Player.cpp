#include "Player.h"
#include <DxLib.h>
#include "Peripheral.h"
#include "Game.h"
#include <algorithm>

#include <random>
#include <string>

void Player::AddCell(const Vector2 & inv)
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

Vector2 Player::PopCell()
{
	Vector2 seed = cellcount.at(0);
	cellcount.erase(cellcount.begin());

	return seed;
}

void Player::LineToDot(const std::vector<Line>& ll)
{
	linedot.resize(0);
	cellcount.resize(0);
	cell.resize(0);

	for (auto line : ll)
	{
		if (line.dir)
		{
			for (int d = line.dotA.x; d < line.dotB.x; d += moveVel)
			{
				Vector2 dot = Vector2(d, line.dotA.y);
				linedot.push_back(dot);
			}
		}
		else
		{
			for (int d = line.dotA.y; d < line.dotB.y; d += moveVel)
			{
				Vector2 dot = Vector2(line.dotA.x, d);
				linedot.push_back(dot);
			}
		}

	}
}

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

	nextSeeds.resize(0);
	seeds.reserve((fieldSize.dotB.x - fieldSize.dotA.x) * (fieldSize.dotB.y - fieldSize.dotA.y));

	
	for (auto& l : outLineLegion)
	{
		for (int y = l.dotA.y; y <= l.dotB.y; ++y)
		{
			for (int x = l.dotA.x; x <= l.dotB.x; ++x)
			{
				seeds.push_back({ Vector2(x, y), 0xff0000 });
			}
		}
	}
	for (int y = (fieldSize.dotA.y + 1); y <= (fieldSize.dotB.y - 1); ++y)
	{
		for (int x = (fieldSize.dotA.x + 1); x <= (fieldSize.dotB.x - 1); ++x)
		{
			seeds.push_back({ Vector2(x, y), 0x000000 });
		}
	}

	// スキャンラインフィルのため
	cell.resize(0);
	cellcount.resize(0);
	linedot.resize(0);

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
	DxLib::DrawExtendGraph(left, up, (right), (down), simg, true);

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

				/*if (dotLegion.size() > 1)
				{
					AddDotAndLine(pos);
					AddPolygon(dotLegion, pos);
					updater = &Player::MoveOn;
				}*/

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
	Draw(pos);
	
	for (auto& d : seeds)
	{
		if (d.color != 0x000000)
		{
			DxLib::DrawPixel(d.pos.x, d.pos.y, d.color);
		}
		/*else
		{
			if (d.pos == pos)
			{
				d.color = 0xff0000;
			}
		}*/
	}


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

//void Player::AddPolygon(std::vector<Vector2>& dotLegion, Vector2& _pos)
//{
//	Line lastline = Line(_pos, dotLegion.at(0));
//	lineLegion.push_back(lastline);
//
//	std::vector<Vector2> dlegion;
//	std::vector<Line> slegion, llegion;		// side, lenght
//	dlegion.resize(0);
//	slegion.resize(0);
//	llegion.resize(0);
//	
//	auto screenSize = Game::Instance().GetScreenSize();
//	slegion.push_back(Line(right, down, left, down));
//	llegion.push_back(Line(right, up, right, down));
//
//	// 縦線と横線に分ける
//	for (auto line : lineLegion)
//	{
//		if (line.dir)
//		{
//			slegion.push_back(line);
//		}
//		else
//		{
//			llegion.push_back(line);
//		}
//	}
//
//	// 左上点さんを探す
//	for (auto line : slegion)
//	{
//		dlegion.push_back(line.dotA);
//	}
//
//	std::sort(dlegion.begin(), dlegion.end(), [](const Vector2& a, const Vector2& b) {return a.y < b.y; });
//	std::sort(dlegion.begin(), dlegion.end(), [](const Vector2& a, const Vector2& b) {return a.x < b.x; });
//	
//	auto originDlegion = dlegion;
//	
//	Vector2 downDot = Vector2(0, 0);
//	Box box;
//
//	// 四角を作っていくよ
//	for (int a = 0; a < dlegion.size(); a++)
//	{
//		auto d = dlegion.at(a);
//		Vector2 sPos = Vector2(d.x, d.y);
//		Vector2 ePos;
//		box.dotA = d;
//		Vector2 end = Vector2(right, down);
//		bool cdflag = false;	// カウントダウンフラグ
//
//		for (ePos.x = (sPos.x + moveVel); ePos.x <= end.x; ePos.x += moveVel)
//		{
//			// 横線判定
//			for(auto sline : slegion)
//			{
//				// 開始点のある横線を探索し、左点をendに
//				if (sline.HitDot(sPos))
//				{
//					end.x = sline.dotB.x;
//					break;
//				}
//			}
//			// 縦線判定
//			for(auto lline : llegion)
//			{
//				// 開始点のある縦線を探索し、下点を削除
//				if (lline.HitDot(sPos))
//				{
//					auto downdot = lline.dotB;
//					for (int d = 0; d < dlegion.size(); d++)
//					{
//						auto dl = dlegion.at(d);
//						if (downdot == dl)
//						{
//							dlegion.erase(dlegion.begin() + d);
//							cdflag = true;
//							break;
//						}
//					}
//				}
//			}
//
//			int count = 0;
//			bool flag = false;
//			for (ePos.y = (sPos.y + moveVel); ePos.y <= end.y; ePos.y += moveVel)
//			{
//				for(auto sline : slegion)
//				{
//					if (sline.HitDot(ePos))
//					{
//						// ラインの右点だったらその点のある縦線の下の点を削除
//						if (ePos == sline.dotB)
//						{
//							for(auto lline : llegion)
//							{
//								if(ePos == lline.dotA)
//								{
//									for (int d = 0; d < dlegion.size(); d++)
//									{
//										auto deld = dlegion.at(d);
//										if (deld == lline.dotB)
//										{
//											dlegion.erase(dlegion.begin() + d);
//											cdflag = true;
//											break;
//										}
//									}
//								}
//							}
//						}
//
//						// ラインの左点でだったらその点を削除
//						if (ePos != sline.dotA)
//						{
//							box = Box(sPos, ePos);
//							boxLegion.push_back(box);
//							flag = true;
//							sPos.x = ePos.x;
//
//							break;
//						}
//						else
//						{
//							if (end.x != ePos.x)
//							{
//								if (count == 0)
//								{
//									// 元のdlegionにあれば最初の点のみ削除
//									for(auto origindot : originDlegion)
//									{
//										if (origindot == ePos)
//										{
//											count++;
//
//											for (int e = 0; e < dlegion.size(); e++)
//											{
//												auto de = dlegion.at(e);
//
//												if (de == origindot)
//												{
//													dlegion.erase(dlegion.begin() + e);
//													cdflag = true;
//												}
//											}
//										}
//									}
//								}
//							}
//						}
//					}
//				}
//				if (flag)
//				{
//					flag = false;
//					break;
//				}
//			}
//		}
//
//		box = Box(sPos.x, sPos.y, ePos.x - moveVel, ePos.y - moveVel);
//		boxLegion.push_back(box);
//
//		if (cdflag)
//		{
//			a = 0;
//		}
//
//	}
//
//	// 四角になってないものを削除
//	for (int a = 0; a < boxLegion.size(); a++)
//	{
//		auto box = boxLegion.at(a);
//		if ((box.dotA.x == box.dotB.x) || (box.dotA.y == box.dotB.y))
//		{
//			boxLegion.erase(boxLegion.begin() + a);
//			a--;
//		}
//
//		for (int b = a + 1; b < boxLegion.size(); b++)
//		{
//			auto boxb = boxLegion.at(b);
//
//			if (box == boxb)
//			{
//				boxLegion.erase(boxLegion.begin() + b);
//				b--;
//			}
//		}
//	}
//
//	// 同じ大きさで隣接してる四角をくっつける
//	for (int a = 0; a < (boxLegion.size() - 1); a++)
//	{
//		auto boxa = boxLegion.at(a);
//		auto boxb = boxLegion.at(a + 1);
//
//		if ((boxa.dotA.y == boxb.dotA.y) && (boxa.dotB.y == boxb.dotB.y))
//		{
//			boxLegion.erase(boxLegion.begin() + a, boxLegion.begin() + (a + 2));
//			boxLegion.insert(boxLegion.begin() + a, Box(boxa.dotA, boxb.dotB));
//			a--;
//		}
//	}
//
//	// 外線に変更
//	for (int a = 0; a < (lineLegion.size() - 1); a++)
//	{
//		Line l = lineLegion.at(a);
//		outLineLegion.push_back(l);
//	}
//
//	lineLegion.resize(0);
//	dotLegion.erase(dotLegion.begin(), dotLegion.end());
//}

void Player::AddPolygon()
{
	// 外線に変更
	/*for (int i = 0; i < (lineLegion.size()); i++)
	{
		Line l = lineLegion.at(i);
		outLineLegion.push_back(l);

		for (int y = l.dotA.y; y <= l.dotB.y; ++y)
		{
			for (int x = l.dotA.x; x <= l.dotB.x; ++x)
			{
				stage.push_back({ Vector2(x, y), 0xff0000 });
			}
		}
	}*/


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
			seeds.at(num).color = 0xffffff;
			flag = false;
		}
	}

	// シードから左右の限界を見つける
	int rx = right, lx = left;
	for (Vector2 rseed = Vector2(++seed.pos.x, seed.pos.y); rseed.x < right; ++rseed.x)
	{
		auto s = std::find(seeds.begin(), seeds.end(), rseed);
		if (s->color != 0x000000)
		{
			//s->color = 0x0000ff;
			rx = rseed.x;
			break;
		}
	}
	for (Vector2 lseed = Vector2(--seed.pos.x, seed.pos.y); lseed.x > left; --lseed.x)
	{
		auto s = std::find(seeds.begin(), seeds.end(), lseed);
		if (s->color != 0x000000)
		{
			//s->color = 0x00ff00;
			lx = lseed.x;
			break;
		}
	}

	// 上で見つけた端から端まで塗りつぶす
	for (int i = lx; i < rx; ++i)
	{

	}

	// 上の1つ上の左右までみて塗れる右側をシード候補に入れる
	for (int i = lx; i < rx; ++i)
	{

	}
	// 上の1つ下の左右までみて塗れる右側をシード候補に入れる
	for (int i = lx; i < rx; ++i)
	{

	}
	// シード候補から1つ取り出してシードに入れる
	// シードが有効なら繰り返す


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
