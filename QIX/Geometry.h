#pragma once

struct Vector2
{
	Vector2() : x(0), y(0) {}
	Vector2(int inx, int iny) : x(inx), y(iny) {}
	int x;
	int y;

	void operator+=(const Vector2& in)
	{
		x += in.x;
		y += in.y;
	}
	void operator-=(const Vector2& in)
	{
		x -= in.x;
		y -= in.y;
	}
	bool operator==(const Vector2& in)const
	{
		return ((x == in.x) && (y == in.y));
	}
	bool operator!=(const Vector2& in)const
	{
		return ((x != in.x) || (y != in.y));
	}
	bool operator|=(const Vector2& in)const
	{
		return ((x == in.x) || (y == in.y));
	}
};

struct Line
{
	Line() : dotA(0, 0), dotB(0, 0), dir(false) {};
	Line(Vector2 invecA, Vector2 invecB);
	Line(int ax, int ay, int bx, int by);

	Vector2 dotA;
	Vector2 dotB;
	bool dir;		// ������T, �c����F

	bool operator==(const Line& inl)const
	{
		return ((inl.dotA == dotA) && (inl.dotB == dotB));
	}
	bool operator!=(const Line& inl)const
	{
		return ((inl.dotA != dotA) || (inl.dotB != dotB));
	}


	// ���̐����Ԃ����Ă��邩�𒲂ׂ�
	bool HitLine(const Line& inl);

	// ���̓_���Ԃ����Ă��邩�𒲂ׂ�
	//@param HidDot...���[������Ɋ܂�
	//@param HitLeftDot...�E�[�͔���Ɋ܂܂Ȃ�
	bool HitDot(const Vector2& inv);
	
	bool HitDoubleDot(const Vector2& adot, const Vector2& bdot);
};

struct Box
{
	Box() : dotA(0, 0), dotB(0, 0) {};
	Box(Vector2 invecA, Vector2 invecB);
	Box(int ax, int ay, int bx, int by);

	Vector2 dotA;
	Vector2 dotB;

	bool operator==(const Box& box)
	{
		return ((box.dotA == dotA) && (box.dotB == dotB));
	}

	// ���̐��Əd�Ȃ��Ă��邩�ۂ�
	//bool OverlapLine(const Line& inl);

};

class Geometry
{
public:
	Geometry();
	~Geometry();
};

