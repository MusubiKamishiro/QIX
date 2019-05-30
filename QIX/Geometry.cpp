#include "Geometry.h"



Geometry::Geometry()
{
}


Geometry::~Geometry()
{
}

Line::Line(Vector2 invecA, Vector2 invecB)
{
	Vector2 a, b;

	if (invecA.x == invecB.x)
	{
		// ècê¸
		if (invecA.y < invecB.y)
		{
			a = invecA;
			b = invecB;
		}
		else
		{
			a = invecB;
			b = invecA;
		}

		dir = false;
	}
	else
	{
		// â°ê¸
		if (invecA.x < invecB.x)
		{
			a = invecA;
			b = invecB;
		}
		else
		{
			a = invecB;
			b = invecA;
		}

		dir = true;
	}

	dotA = a;
	dotB = b;
}

Line::Line(int ax, int ay, int bx, int by)
{
	Vector2 a, b;

	if (ax == bx)
	{
		// ècê¸
		if (ay < by)
		{
			a = Vector2(ax, ay);
			b = Vector2(bx, by);
		}
		else
		{
			a = Vector2(bx, by);
			b = Vector2(ax, ay);
		}

		dir = false;
	}
	else
	{
		// â°ê¸
		if (ax < bx)
		{
			a = Vector2(ax, ay);
			b = Vector2(bx, by);
		}
		else
		{
			a = Vector2(bx, by);
			b = Vector2(ax, ay);
		}

		dir = true;
	}
	dotA = a;
	dotB = b;
}

bool Line::HitLine(const Line& inl)
{
	if ((((dotA.x <= inl.dotA.x) && (dotB.x >= inl.dotA.x)) || ((dotA.y <= inl.dotA.y) && (dotB.y >= inl.dotA.y))) &&
		(((dotA.x <= inl.dotB.x) && (dotB.x >= inl.dotB.x)) || ((dotA.y <= inl.dotB.y) && (dotB.y >= inl.dotB.y))) &&
		(((inl.dotA.x <= dotA.x) && (inl.dotB.x >= dotA.x)) || ((inl.dotA.y <= dotA.y) && (inl.dotB.y >= dotA.y))) &&
		(((inl.dotA.x <= dotB.x) && (inl.dotB.x >= dotB.x)) || ((inl.dotA.y <= dotB.y) && (inl.dotB.y >= dotB.y))))
	{
		return true;
	}

	return false;
}

bool Line::HitDot(const Vector2& inv)
{
	if ((((dotA.x <= inv.x) && (dotB.x >= inv.x)) && ((dotA.y == inv.y) && (dotB.y == inv.y))) || 
		(((dotA.y <= inv.y) && (dotB.y >= inv.y)) && ((dotA.x == inv.x) && (dotB.x == inv.x))))
	{
		return true;
	}

	return false;
}

bool Line::HitDoubleDot(const Vector2& adot, const Vector2& bdot)
{
	if (HitDot(adot) && HitDot(bdot))
	{
		return true;
	}
	return false;
}

Box::Box(Vector2 invecA, Vector2 invecB)
{
	if (invecA.x < invecB.x)
	{
		if (invecA.y < invecB.y)
		{
			dotA = invecA;
			dotB = invecB;
		}
		else
		{
			dotA = Vector2(invecA.x, invecB.y);
			dotB = Vector2(invecB.x, invecA.y);
		}
	}
	else
	{
		if (invecA.y < invecB.y)
		{
			dotA = Vector2(invecB.x, invecA.y);
			dotB = Vector2(invecA.x, invecB.y);
		}
		else
		{
			dotA = invecB;
			dotB = invecA;
		}
	}
}

Box::Box(int ax, int ay, int bx, int by)
{
	if (ax < bx)
	{
		if (ay < by)
		{
			dotA = Vector2(ax, ay);
			dotB = Vector2(bx, by);
		}
		else
		{
			dotA = Vector2(ax, by);
			dotB = Vector2(bx, ay);
		}
	}
	else
	{
		if (ay < by)
		{
			dotA = Vector2(bx, ay);
			dotB = Vector2(ax, by);
		}
		else
		{
			dotA = Vector2(bx, by);
			dotB = Vector2(ax, ay);
		}

	}
}

/*bool Box::OverlapLine(const Line & inl)
{
	Line line;

	if (inl.dir)
	{
		// è„ê¸
		line = Line(dotA.x, dotA.y, dotB.x, dotA.y);
		if ((inl.dotA.x <= line.dotA.x) && (inl.dotB.x >= line.dotB.x))
		{
			return true;
		}

		// â∫ê¸
		line = Line(dotA.x, dotB.y, dotB.x, dotB.y);
		if ((inl.dotA.x <= line.dotA.x) && (inl.dotB.x >= line.dotB.x))
		{
			return true;
		}
	}
	else
	{
		// âEê¸
		line = Line(dotB.x, dotA.y, dotB.x, dotB.y);
		if ((inl.dotA.y <= line.dotA.y) && (inl.dotB.y >= line.dotB.y))
		{
			return true;
		}

		// ç∂ê¸
		line = Line(dotA.x, dotA.y, dotA.x, dotB.y);
		if ((inl.dotA.y <= line.dotA.y) && (inl.dotB.y >= line.dotB.y))
		{
			return true;
		}
	}
	

	return false;
}*/
