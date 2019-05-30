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
	std::random_device rd;
	std::mt19937 mt(rd());

	int num;
	
	std::string s = "img/" + std::to_string((mt() % 8) + 1) + ".png";

	//img = DxLib::LoadGraph(s.c_str());
	img = DxLib::LoadGraph("img/yojo.png");
	simg = DxLib::LoadGraph("img/yojo0.png");

	auto screenSize = Game::Instance().GetScreenSize();
	auto fieldSize = Game::Instance().GetFieldSize();

	up = fieldSize.dotA.y;
	right = fieldSize.dotB.x;
	left = fieldSize.dotA.x;
	down = fieldSize.dotB.y;

	BG = DxLib::MakeScreen((right - left), (down - up));


	dotLegion.resize(0);
	outDotLegion.resize(0);
	lineLegion.resize(0);
	outLineLegion.resize(0);
	boxLegion.resize(0);

	// 外点
	outDotLegion.push_back(Vector2(left, up));
	outDotLegion.push_back(Vector2(right, up));
	outDotLegion.push_back(Vector2(right, down));
	outDotLegion.push_back(Vector2(left, down));

	// 外線
	outLineLegion.push_back(Line(0, up, screenSize.x, up));
	outLineLegion.push_back(Line(0, down, screenSize.x, down));
	outLineLegion.push_back(Line(left, up, left, down));
	outLineLegion.push_back(Line(right, up, right, down));

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
		int size = outLineLegion.size();
		for (int a = 0; a < size; a++)
		{
			auto b = outLineLegion.at(a);
			if (b.HitDot(pos))
			{
				startPos = pos;

				if (dotLegion.size() > 1)
				{
					AddDotAndLine(pos);
					AddPolygon(dotLegion, pos);
					//AddPolygon(lineLegion, pos);
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

void Player::AddPolygon(std::vector<Vector2>& dotLegion, Vector2& _pos)
{
	Line lastline = Line(_pos, dotLegion.at(0));
	lineLegion.push_back(lastline);

	std::vector<Vector2> dlegion;
	std::vector<Line> slegion, llegion;		// side, lenght
	dlegion.resize(0);
	slegion.resize(0);
	llegion.resize(0);
	
	auto screenSize = Game::Instance().GetScreenSize();
	slegion.push_back(Line(right, down, left, down));
	llegion.push_back(Line(right, up, right, down));

	// 縦線と横線に分ける
	for (auto line : lineLegion)
	{
		if (line.dir)
		{
			slegion.push_back(line);
		}
		else
		{
			llegion.push_back(line);
		}
	}

	// 左上点さんを探す
	for (auto line : slegion)
	{
		dlegion.push_back(line.dotA);
	}

	std::sort(dlegion.begin(), dlegion.end(), [](const Vector2& a, const Vector2& b) {return a.y < b.y; });
	std::sort(dlegion.begin(), dlegion.end(), [](const Vector2& a, const Vector2& b) {return a.x < b.x; });
	
	auto originDlegion = dlegion;
	
	Vector2 downDot = Vector2(0, 0);
	Box box;

	// 四角を作っていくよ
	for (int a = 0; a < dlegion.size(); a++)
	{
		auto d = dlegion.at(a);
		Vector2 sPos = Vector2(d.x, d.y);
		Vector2 ePos;
		box.dotA = d;
		Vector2 end = Vector2(right, down);
		bool cdflag = false;	// カウントダウンフラグ

		for (ePos.x = (sPos.x + moveVel); ePos.x <= end.x; ePos.x += moveVel)
		{
			// 横線判定
			for(auto sline : slegion)
			{
				// 開始点のある横線を探索し、左点をendに
				if (sline.HitDot(sPos))
				{
					end.x = sline.dotB.x;
					break;
				}
			}
			// 縦線判定
			for(auto lline : llegion)
			{
				// 開始点のある縦線を探索し、下点を削除
				if (lline.HitDot(sPos))
				{
					auto downdot = lline.dotB;
					for (int d = 0; d < dlegion.size(); d++)
					{
						auto dl = dlegion.at(d);
						if (downdot == dl)
						{
							dlegion.erase(dlegion.begin() + d);
							cdflag = true;
							break;
						}
					}
				}
			}

			int count = 0;
			bool flag = false;
			for (ePos.y = (sPos.y + moveVel); ePos.y <= end.y; ePos.y += moveVel)
			{
				for(auto sline : slegion)
				{
					if (sline.HitDot(ePos))
					{
						// ラインの右点だったらその点のある縦線の下の点を削除
						if (ePos == sline.dotB)
						{
							for(auto lline : llegion)
							{
								if(ePos == lline.dotA)
								{
									for (int d = 0; d < dlegion.size(); d++)
									{
										auto deld = dlegion.at(d);
										if (deld == lline.dotB)
										{
											dlegion.erase(dlegion.begin() + d);
											cdflag = true;
											break;
										}
									}
								}
							}
						}

						// ラインの左点でだったらその点を削除
						if (ePos != sline.dotA)
						{
							box = Box(sPos, ePos);
							boxLegion.push_back(box);
							flag = true;
							sPos.x = ePos.x;

							break;
						}
						else
						{
							if (end.x != ePos.x)
							{
								if (count == 0)
								{
									// 元のdlegionにあれば最初の点のみ削除
									for(auto origindot : originDlegion)
									{
										if (origindot == ePos)
										{
											count++;

											for (int e = 0; e < dlegion.size(); e++)
											{
												auto de = dlegion.at(e);

												if (de == origindot)
												{
													dlegion.erase(dlegion.begin() + e);
													cdflag = true;
												}
											}
										}
									}
								}
							}
						}
					}
				}
				if (flag)
				{
					flag = false;
					break;
				}
			}
		}

		box = Box(sPos.x, sPos.y, ePos.x - moveVel, ePos.y - moveVel);
		boxLegion.push_back(box);

		if (cdflag)
		{
			a = 0;
		}

	}

	// 四角になってないものを削除
	for (int a = 0; a < boxLegion.size(); a++)
	{
		auto box = boxLegion.at(a);
		if ((box.dotA.x == box.dotB.x) || (box.dotA.y == box.dotB.y))
		{
			boxLegion.erase(boxLegion.begin() + a);
			a--;
		}

		for (int b = a + 1; b < boxLegion.size(); b++)
		{
			auto boxb = boxLegion.at(b);

			if (box == boxb)
			{
				boxLegion.erase(boxLegion.begin() + b);
				b--;
			}
		}
	}

	// 同じ大きさで隣接してる四角をくっつける
	for (int a = 0; a < (boxLegion.size() - 1); a++)
	{
		auto boxa = boxLegion.at(a);
		auto boxb = boxLegion.at(a + 1);

		if ((boxa.dotA.y == boxb.dotA.y) && (boxa.dotB.y == boxb.dotB.y))
		{
			boxLegion.erase(boxLegion.begin() + a, boxLegion.begin() + (a + 2));
			boxLegion.insert(boxLegion.begin() + a, Box(boxa.dotA, boxb.dotB));
			a--;
		}
	}

	// 外線に変更
	for (int a = 0; a < (lineLegion.size() - 1); a++)
	{
		Line l = lineLegion.at(a);
		outLineLegion.push_back(l);
	}

	lineLegion.resize(0);
	dotLegion.erase(dotLegion.begin(), dotLegion.end());
}

void Player::AddPolygon(std::vector<Line>& llegion, Vector2& _pos)
{
	Vector2 start;
	
	auto s = lineLegion.at(lineLegion.size() - 1);

	// 塗りつぶしのスタート地点を決める
	if (s.dir)
	{
		if ((_pos.y - moveVel) > up)
		{
			start = Vector2(_pos.x, (_pos.y - moveVel));
		}
		else if ((_pos.y + moveVel) < down)
		{
			start = Vector2(_pos.x, (_pos.y + moveVel));
		}
	}
	else
	{
		if ((_pos.x + moveVel) < right)
		{
			start = Vector2((_pos.x + moveVel), _pos.y);
		}
		else if ((_pos.x - moveVel) > left)
		{
			start = Vector2((_pos.x - moveVel), _pos.y);
		}
	}

	
	// 引いた線を点に変える
	LineToDot(llegion);

	// 開始点を加える
	AddCell(start);

	// とりあえずつぶす
	while (cellcount.size() > 0)
	{
		Vector2 popup = PopCell();

		int count = 0;
		for(int a = 0; a < linedot.size(); a++)
		{
			auto dot = linedot.at(a);
			
			if ((Vector2((popup.x + moveVel), popup.y) == dot) && ((popup.x + moveVel) < right))
			{
				int c = 0;
				/*for (auto dotb : linedot)
				{
					if ((Vector2((popup.x - moveVel), popup.y) == dotb) && ((popup.x - moveVel) >= left))
					{
						c++;
					}
					else if ((Vector2(popup.x, (popup.y + moveVel)) == dotb) && ((popup.y + moveVel) < down))
					{
						c++;
					}
					else if ((Vector2(popup.x, (popup.y - moveVel)) == dotb) && ((popup.y - moveVel) >= up))
					{
						c++;
					}
				}*/

				/*if (!c)
				{
					if ((popup.x - moveVel) >= left)
					{
						AddCell(Vector2((popup.x - moveVel), popup.y));
					}
					if ((popup.y + moveVel) < down)
					{
						AddCell(Vector2(popup.x, (popup.y + moveVel)));
					}
					if ((popup.y - moveVel) >= up)
					{
						AddCell(Vector2(popup.x, (popup.y - moveVel)));
					}
				}*/
				
				count++;
			}
			if ((Vector2((popup.x - moveVel), popup.y) == dot) && ((popup.x - moveVel) >= left))
			{
				int c = 0;
				/*for (auto dotb : linedot)
				{
					if ((Vector2((popup.x + moveVel), popup.y) == dotb) && ((popup.x + moveVel) < right))
					{
						c++;
					}
					else if ((Vector2(popup.x, (popup.y + moveVel)) == dotb) && ((popup.y + moveVel) < down))
					{
						c++;
					}
					else if ((Vector2(popup.x, (popup.y - moveVel)) == dotb) && ((popup.y - moveVel) >= up))
					{
						c++;
					}
				}*/

				/*if (!c)
				{
					if ((popup.x + moveVel) < right)
					{
						AddCell(Vector2((popup.x + moveVel), popup.y));
					}
					if ((popup.y + moveVel) < down)
					{
						AddCell(Vector2(popup.x, (popup.y + moveVel)));
					}
					if ((popup.y - moveVel) >= up)
					{
						AddCell(Vector2(popup.x, (popup.y - moveVel)));
					}
				}*/
				count++;
			}
			if ((Vector2(popup.x, (popup.y + moveVel)) == dot) && ((popup.y + moveVel) < down))
			{
				int c = 0;
				/*for (auto dotb : linedot)
				{
					if ((Vector2((popup.x + moveVel), popup.y) == dotb) && ((popup.x + moveVel) < right))
					{
						c++;
					}
					else if ((Vector2((popup.x - moveVel), popup.y) == dotb) && ((popup.x - moveVel) >= left))
					{
						c++;
					}
					else if ((Vector2(popup.x, (popup.y - moveVel)) == dotb) && ((popup.y - moveVel) >= up))
					{
						c++;
					}
				}*/

				/*if (!c)
				{
					if ((popup.x + moveVel) < right)
					{
						AddCell(Vector2((popup.x + moveVel), popup.y));
					}
					if ((popup.x - moveVel) >= left)
					{
						AddCell(Vector2((popup.x - moveVel), popup.y));
					}
					if ((popup.y - moveVel) >= up)
					{
						AddCell(Vector2(popup.x, (popup.y - moveVel)));
					}
				}*/
				count++;
			}		
			if ((Vector2(popup.x, (popup.y - moveVel)) == dot) && ((popup.y - moveVel) >= up))
			{
				int c = 0;
				/*for (auto dotb : linedot)
				{
					if ((Vector2((popup.x + moveVel), popup.y) == dotb) && ((popup.x + moveVel) < right))
					{
						c++;
					}
					else if ((Vector2((popup.x - moveVel), popup.y) == dotb) && ((popup.x - moveVel) >= left))
					{
						c++;
					}
					else if ((Vector2(popup.x, (popup.y + moveVel)) == dotb) && ((popup.y + moveVel) < down))
					{
						c++;
					}
				}*/

				/*if (!c)
				{
					if ((popup.x + moveVel) < right)
					{
						AddCell(Vector2((popup.x + moveVel), popup.y));
					}
					if ((popup.x - moveVel) >= left)
					{
						AddCell(Vector2((popup.x - moveVel), popup.y));
					}
					if ((popup.y + moveVel) < down)
					{
						AddCell(Vector2(popup.x, (popup.y + moveVel)));
					}
				}*/
				count++;
			}

			if (count)
			{
				break;
			}
		}
		if (!count)
		{
			if ((popup.x + moveVel) <= right)
			{
				AddCell(Vector2((popup.x + moveVel), popup.y));
			}
			if ((popup.x - moveVel) >= left)
			{
				AddCell(Vector2((popup.x - moveVel), popup.y));
			}
			if ((popup.y + moveVel) <= down)
			{
				AddCell(Vector2(popup.x, (popup.y + moveVel)));
			}
			if ((popup.y - moveVel) >= up)
			{
				AddCell(Vector2(popup.x, (popup.y - moveVel)));
			}
		}
	}

	// つぶしたところで四角をつくる
	//std::sort(cell.begin(), cell.end(), [](const Vector2& a, const Vector2& b) {return a.y < b.y; });
	//std::sort(cell.begin(), cell.end(), [](const Vector2& a, const Vector2& b) {return a.x < b.x; });

	for (auto dot : cell)
	{
		Box b = Box(dot, Vector2(dot.x + moveVel, dot.y + moveVel));
		boxLegion.push_back(b);
	}
	
	Line lastline = Line(_pos, dotLegion.at(0));
	lineLegion.push_back(lastline);

	// 外線に変更
	for (int a = 0; a < (lineLegion.size() - 1); a++)
	{
		Line l = lineLegion.at(a);
		outLineLegion.push_back(l);
	}

	lineLegion.resize(0);
	dotLegion.erase(dotLegion.begin(), dotLegion.end());
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
}

bool Player::NotOutOfRange()
{
	int count = 0;

	// 範囲外にはいかせないぜ
	if (pos.x <= left)
	{
		pos.x = left;
		count++;
	}
	else if (pos.x >= right)
	{
		pos.x = right;
		count++;
	}

	if (pos.y <= up)
	{
		pos.y = up;
		count++;
	}
	else if (pos.y >= down)
	{
		pos.y = down;
		count++;
	}

	if (count)
	{
		return true;
	}

	return false;
}
