#include "GamePlayingScene.h"
#include <DxLib.h>
#include <stdlib.h>
#include "../Peripheral.h"
#include "../Game.h"
#include "TitleScene.h"

#include "../TitleLogo.h"
#include "../Player.h"


void GamePlayingScene::FadeinUpdate(const Peripheral & p)
{
	if (p.IsTrigger(PAD_INPUT_8))
	{
		pal = 255;
		updater = &GamePlayingScene::FadeoutUpdate;
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

void GamePlayingScene::FadeoutUpdate(const Peripheral & p)
{
	if (pal <= 0)
	{
		Game::Instance().ChangeScene(new TitleScene());
	}
	else
	{
		pal -= 20;
	}
}

GamePlayingScene::GamePlayingScene()
{
	player.reset(new Player());
	tLogo.reset(new TitleLogo());

	ssize = Game::Instance().GetScreenSize();

	updater = &GamePlayingScene::FadeinUpdate;
}


GamePlayingScene::~GamePlayingScene()
{
}

void GamePlayingScene::Update(const Peripheral& p)
{
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	tLogo->LogoDraw();

	player->Update(p);


	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::abs(pal - 255));
	DxLib::DrawBox(0, 0, ssize.x, ssize.y, 0x000000, true);
	(this->*updater)(p);
}
