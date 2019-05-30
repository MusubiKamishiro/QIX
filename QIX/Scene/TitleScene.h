#pragma once
#include "Scene.h"

class TitleScene : public Scene
{
private:
	int titleImage;	// À²ÄÙ‰æ‘œ

	void (TitleScene::*updater)(const Peripheral& p);

	void FadeinUpdate(const Peripheral& p);
	void FadeoutUpdate(const Peripheral& p);

public:
	TitleScene();
	~TitleScene();

	void Update(const Peripheral& p);
};

