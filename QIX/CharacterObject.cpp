#include "CharacterObject.h"



CharacterObject::CharacterObject()
{
	vel = Vector2(0, 0);
	pos = Vector2(10, 50);
}


CharacterObject::~CharacterObject()
{
}

Vector2 CharacterObject::GetPos()const
{
	return pos;
}
