#include "TitleLogo.h"
#include <DxLib.h>


TitleLogo::TitleLogo()
{
}


TitleLogo::~TitleLogo()
{
}

void TitleLogo::LogoDraw()
{
	// É^ÉCÉgÉãÉçÉS
	// Q
	DxLib::DrawBox(20, 10, 45, 35, 0xfffffff, false);
	DxLib::DrawLine(35, 25, 50, 40, 0xffffff);
	// _
	DxLib::DrawLine(50, 40, 110, 40, 0xffffff);
	DxLib::DrawLine(100, 40, 115, 40, 0xd0d0d0);
	DxLib::DrawLine(115, 40, 120, 40, 0xa0a0a0);
	// I
	DxLib::DrawLine(60, 10, 70, 10, 0xffffff);
	DxLib::DrawLine(65, 10, 65, 35, 0xffffff);
	DxLib::DrawLine(60, 35, 70, 35, 0xffffff);
	// X
	DxLib::DrawLine(85, 10, 105, 35, 0xffffff);
	DxLib::DrawLine(105, 10, 85, 35, 0xffffff);
	// <<<
	DxLib::DrawLine(110, 10, 100, 23, 0xd0d0d0);
	DxLib::DrawLine(100, 23, 110, 35, 0xd0d0d0);
	DxLib::DrawLine(115, 10, 105, 23, 0xd0d0d0);
	DxLib::DrawLine(105, 23, 115, 35, 0xd0d0d0);
	DxLib::DrawLine(120, 10, 110, 23, 0xa0a0a0);
	DxLib::DrawLine(110, 23, 120, 35, 0xa0a0a0);
}
