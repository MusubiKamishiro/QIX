#include "Game.h"
#include <DxLib.h>
#include "Peripheral.h"

#include "Scene/TitleScene.h"
//#include "resource.h"


Game::Game() : ScreenSize(720, 450)//(720, 450)
{
	field = Box(10, 50, (ScreenSize.x - 10), (ScreenSize.y - 20));
	time = fps = count = oldcount = 0;
}

void Game::operator=(const Game &)
{
}


Game::~Game()
{
}

void Game::Initialize()
{
	DxLib::ChangeWindowMode(true);
	DxLib::SetGraphMode(ScreenSize.x, ScreenSize.y, 32);

	if (DxLib::DxLib_Init() == -1)
	{
		return;
	}

	DxLib::SetMainWindowText("QIX");		// タイトル
	//DxLib::SetWindowIconID(IDI_ICON1);	// アイコン
	DxLib::SetDrawScreen(DX_SCREEN_BACK);	// 裏画面に描画

	ChangeScene(new TitleScene());
}

void Game::Run()
{
	Peripheral peripheral;

	while (DxLib::ProcessMessage() == 0)
	{
		++time;
		DxLib::ClearDrawScreen();

		if (DxLib::CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}
		peripheral.Update();

		count = DxLib::GetNowCount();
		if ((count - oldcount) > 1000)
		{
			fps = ((time * 1000) / (count - oldcount));
			oldcount = count;
			time = 0;
		}

		scene->Update(peripheral);

		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		DxLib::DrawFormatString(ScreenSize.x - 100, ScreenSize.y - 20, 0xff00ff, "%.2f fps", fps);

		DxLib::ScreenFlip();
	}
}

void Game::Terminate()
{
	DxLib::DxLib_End();
}

void Game::ChangeScene(Scene* s)
{
	scene.reset(s);
}

const Vector2& Game::GetScreenSize()const
{
	return ScreenSize;
}

const Box & Game::GetFieldSize() const
{
	return field;
}
