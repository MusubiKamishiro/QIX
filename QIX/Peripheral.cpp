#include "Peripheral.h"
#include <DxLib.h>



Peripheral::Peripheral()
{
}


Peripheral::~Peripheral()
{
}

void Peripheral::Update()
{
	lastPadState = padState;
	padState = DxLib::GetJoypadInputState(DX_INPUT_KEY_PAD1);
}

bool Peripheral::IsPressing(int keyid) const
{
	return (padState & keyid);
}

bool Peripheral::IsTrigger(int keyid) const
{
	return (!(lastPadState & keyid) && (padState & keyid));
}


