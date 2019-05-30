#pragma once
#include "Geometry.h"

class CharacterObject
{
protected:
	Vector2 vel;
	Vector2 pos;

public:
	CharacterObject();
	~CharacterObject();

	Vector2 GetPos()const;
};

