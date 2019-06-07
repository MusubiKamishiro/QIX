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
	// �摜�������_���Ō���
	/*std::random_device rd;
	std::mt19937 mt(rd());

	int num;
	
	std::string s = "img/" + std::to_string((mt() % 8) + 1) + ".png";*/
	//img = DxLib::LoadGraph(s.c_str());
	img = DxLib::LoadGraph("img/bg1.png");		// �Q�[����i�߂�ƌ���鍷��(���J��)�摜
	simg = DxLib::LoadGraph("img/bg2.png");		// �ŏ��̉摜

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

	// �O��
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

	// �X�L�������C���t�B���̂���
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
	// �h��Ԃ��ŏo�Ă���摜
	DxLib::SetDrawScreen(BG);
	DxLib::DrawExtendGraph(0, 0, (right - left), (down - up), img, true);
	// �ŏ��Ɍ�����摜
	DxLib::SetDrawScreen(DX_SCREEN_BACK);		// �`����߂�
	DxLib::DrawExtendGraph(left, up, (right), (down), simg, true);

	// �ړ����������܂�
	(this->*updater)(p);

	if ((p.IsTrigger(PAD_INPUT_1)) && (updater == &Player::MoveOn))
	{
		startPos = pos;
		dotLegion.push_back(startPos);
		updater = &Player::DrawLineMove;
	}

	// �O�t���[���̈ړ������ƈ�����璸�_�̒ǉ�
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
	// �{�^������������ړ�(����͎΂߈ړ��֎~)
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

	// �s���悪����łȂ���Έړ������Ȃ�
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
	// �_�̒ǉ�
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
//	// �c���Ɖ����ɕ�����
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
//	// ����_�����T��
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
//	// �l�p������Ă�����
//	for (int a = 0; a < dlegion.size(); a++)
//	{
//		auto d = dlegion.at(a);
//		Vector2 sPos = Vector2(d.x, d.y);
//		Vector2 ePos;
//		box.dotA = d;
//		Vector2 end = Vector2(right, down);
//		bool cdflag = false;	// �J�E���g�_�E���t���O
//
//		for (ePos.x = (sPos.x + moveVel); ePos.x <= end.x; ePos.x += moveVel)
//		{
//			// ��������
//			for(auto sline : slegion)
//			{
//				// �J�n�_�̂��鉡����T�����A���_��end��
//				if (sline.HitDot(sPos))
//				{
//					end.x = sline.dotB.x;
//					break;
//				}
//			}
//			// �c������
//			for(auto lline : llegion)
//			{
//				// �J�n�_�̂���c����T�����A���_���폜
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
//						// ���C���̉E�_�������炻�̓_�̂���c���̉��̓_���폜
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
//						// ���C���̍��_�ł������炻�̓_���폜
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
//									// ����dlegion�ɂ���΍ŏ��̓_�̂ݍ폜
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
//	// �l�p�ɂȂ��ĂȂ����̂��폜
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
//	// �����傫���ŗאڂ��Ă�l�p����������
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
//	// �O���ɕύX
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
	// �O���ɕύX
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

	// �V�[�h��T��
	Seed seed;
	// �V�[�h�̊J�n�ʒu�����߂�
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

	// �V�[�h���獶�E�̌��E��������
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

	// ��Ō������[����[�܂œh��Ԃ�
	for (int i = lx; i < rx; ++i)
	{

	}

	// ���1��̍��E�܂ł݂ēh���E�����V�[�h���ɓ����
	for (int i = lx; i < rx; ++i)
	{

	}
	// ���1���̍��E�܂ł݂ēh���E�����V�[�h���ɓ����
	for (int i = lx; i < rx; ++i)
	{

	}
	// �V�[�h��₩��1���o���ăV�[�h�ɓ����
	// �V�[�h���L���Ȃ�J��Ԃ�


	lineLegion.erase(lineLegion.begin(), lineLegion.end());
	dotLegion.erase(dotLegion.begin(), dotLegion.end());
	lineLegion.resize(0);
	dotLegion.resize(0);
}


void Player::LineDraw()
{
	// ���̏W�������ɕ`��
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
	// ���̏W�������ɕ`��
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
	std::string s = "box�̐��F";
	s += std::to_string(boxLegion.size());
	DxLib::DrawString(550, 20, s.c_str(), 0x00ffff);

	s = "���m����̐��F";
	s += std::to_string(lineLegion.size());
	DxLib::DrawString(550, 40, s.c_str(), 0x00ffff);

	s = "�m����̐��F";
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

	// �͈͊O�ɂ͂������Ȃ���
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
