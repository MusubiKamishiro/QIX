#pragma once
#include "Geometry.h"
#include "CharacterObject.h"
#include <memory>
#include <vector>

struct Seed
{
	Vector2 pos;	// �_�̍��W
	int color;		// �h���Ă�F

	bool operator==(const Vector2& inv)
	{
		return (pos == inv);
	}
};

enum Dir
{
	DIR_UP,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT,
	DIR_MAX
};

class Peripheral;

class Player
{
private:
	// ��ԑJ�ڂ̂��߂̃����o�֐��|�C���^
	void (Player::*updater)(const Peripheral &p);
	void Move(const Peripheral &p);
	void MoveOn(const Peripheral &p);			// ���̏���ړ�
	void DrawLineMove(const Peripheral &p);		// ���������Ȃ���ړ�
	void Die(const Peripheral &p);				// �G�@�ɓ������Ď���

	void Draw(Vector2& pos);					// �v���C���[�̕`��
	void AddDotAndLine(Vector2& pos);
	//void AddPolygon(std::vector<Vector2>& dotLegion, Vector2& _pos);
	void AddPolygon();
	void LineDraw();
	void OutLineDraw();
	void PolygonDraw();

	void DebugDraw();
	
	bool NotOutOfRange();

	Vector2 startPos;	// �J�n���W
	Vector2 pos;		// ���ݍ��W
	Vector2 vel;		// �ړ����x
	int moveVel;		// �ړ���
	int moveDir;		// �ړ�����
	int oldMoveDir;		// 1�O�̈ړ�����

	std::vector<Vector2> dotLegion;		// �_�̌R��
	std::vector<Line> lineLegion;		// ���̌R��
	std::vector<Line> outLineLegion;	// �O���̌R��
	std::vector<Box> boxLegion;			// �l�p�`�̌R��

	std::vector<Seed> seeds;
	std::vector<Seed> nextSeeds;

	std::vector<Vector2> cell;			// �h��Ԃ��ꏊ
	std::vector<Vector2> cellcount;
	std::vector<Vector2> linedot;		// �����̓_

	void AddCell(const Vector2& inv);	// �ǉ�
	Vector2 PopCell();					// ���o��

	void LineToDot(const std::vector<Line>& ll);		// ������_��

	// �ړ��͈͓�
	int up;
	int right;
	int left;
	int down;

	int img;
	int BG;
	int simg;

public:
	Player();
	~Player();

	void Update(const Peripheral &p);

	Vector2 GetPos()const;
};

