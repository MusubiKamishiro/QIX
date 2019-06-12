#pragma once
#include "Geometry.h"
#include "CharacterObject.h"
#include <memory>
#include <vector>
#include <array>


struct Seed
{
	Vector2 pos;	// 点の座標
	int color;		// 塗られてる色

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
	// 状態遷移のためのメンバ関数ポインタ
	void (Player::*updater)(const Peripheral &p);
	void Move(const Peripheral &p);
	void MoveOn(const Peripheral &p);			// 線の上を移動
	void DrawLineMove(const Peripheral &p);		// 線を引きながら移動
	void Die(const Peripheral &p);				// 敵機に当たって死んだ

	void Draw(Vector2& pos);					// プレイヤーの描画
	void AddDotAndLine(Vector2& pos);
	void AddPolygon();
	void LineDraw();
	void OutLineDraw();
	void PolygonDraw();

	void DebugDraw();
	
	bool NotOutOfRange();

	Vector2 startPos;	// 開始座標
	Vector2 pos;		// 現在座標
	Vector2 vel;		// 移動速度
	int moveVel;		// 移動量
	int moveDir;		// 移動方向
	int oldMoveDir;		// 1つ前の移動方向

	std::vector<Vector2> dotLegion;		// 点の軍勢
	std::vector<Line> lineLegion;		// 線の軍勢
	std::vector<Line> outLineLegion;	// 外線の軍勢
	std::vector<Box> boxLegion;			// 四角形の軍勢

	std::array<Seed, ((710-10 + 1) * (430-50 + 1))> seeds;
	//10, 50, 720 - 10, 450 - 20

		// 移動範囲内
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

