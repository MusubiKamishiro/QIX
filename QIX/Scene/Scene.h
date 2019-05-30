#pragma once


class Peripheral;

// (純粋仮想ｸﾗｽ)
class Scene
{
public:
	Scene();
	virtual ~Scene();

	// ｼｰﾝの更新を行う
	virtual void Update(const Peripheral& p) = 0;
	
	virtual void FadeinUpdate(const Peripheral& p) = 0;
	virtual void FadeoutUpdate(const Peripheral& p) = 0;

	int pal;
};

