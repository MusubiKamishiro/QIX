#include <DxLib.h>
#include "Game.h"


int main(void)
{
	Game& game = Game::Instance();
	game.Initialize();
	game.Run();
	game.Terminate();
}

