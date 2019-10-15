#pragma once
#include "Scene.h"

class TitleScene : public Scene
{
private:
	int titleImage;	// ���ى摜

	void (TitleScene::*updater)(const Peripheral& p);

	void FadeinUpdate(const Peripheral& p);
	void FadeoutUpdate(const Peripheral& p);

public:
	TitleScene();
	~TitleScene();

	void Update(const Peripheral& p);
};

