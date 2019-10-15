#include "TitleScene.h"
#include <DxLib.h>
#include "../Peripheral.h"
#include "../Game.h"
#include "GamePlayingScene.h"

void TitleScene::FadeinUpdate(const Peripheral & p)
{
	if (p.IsTrigger(PAD_INPUT_8))
	{
		pal = 255;
		updater = &TitleScene::FadeoutUpdate;
	}
	
	if (pal >= 255)
	{
		pal = 255;
	}
	else
	{
		pal += 20;
	}
}

void TitleScene::FadeoutUpdate(const Peripheral & p)
{
	if (pal <= 0)
	{
		Game::Instance().ChangeScene(new GamePlayingScene());
	}
	else
	{
		pal -= 20;
	}
}

TitleScene::TitleScene()
{
	titleImage = DxLib::LoadGraph("img/yoshiki.png");
	updater = &TitleScene::FadeinUpdate;
}


TitleScene::~TitleScene()
{
}

void TitleScene::Update(const Peripheral& p)
{
	(this->*updater)(p);

	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, pal);
	DxLib::DrawExtendGraph(0, 0, Game::Instance().GetScreenSize().x, Game::Instance().GetScreenSize().y, titleImage, true);
}
