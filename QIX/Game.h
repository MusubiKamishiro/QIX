#pragma once
#include <memory>
#include "Scene/Scene.h"
#include "Geometry.h"


// ｹﾞｰﾑ全体を制御するｸﾗｽ
class Game
{
private:
	// ｼﾝｸﾞﾙﾄﾝの決まり
	Game();							// 生成禁止
	Game(const Game&);				// ｺﾋﾟｰ禁止
	void operator=(const Game&);	// 代入禁止

	const Vector2 ScreenSize;	// 画面サイズ
	Box field;					// プレイヤーが移動可能な範囲
	float time, fps, count, oldcount;	// fps表示の際に使用する


public:
	// ｼﾝｸﾞﾙﾄﾝｲﾝﾀｰﾌｪｰｽ
	static Game& Instance()
	{
		static Game instance;
		return instance;
	}
	~Game();

	// 初期化系
	// 基本素材のﾛｰﾄﾞとか初期処理を行う
	void Initialize();

	// 中にﾙｰﾌﾟあり
	// これを呼ぶとｹﾞｰﾑが始まる
	void Run();

	// 後処理
	// ﾗｲﾌﾞﾗﾘの後処理や
	void Terminate();

	// 現在実行中のｼｰﾝの管理
	std::shared_ptr<Scene> scene;

	// ｼｰﾝ入れ替え
	void ChangeScene(Scene* s);

	const Vector2& GetScreenSize()const;
	const Box& GetFieldSize()const;
};

