#pragma once
#include <memory>
#include "Scene/Scene.h"
#include "Geometry.h"


// ¹Ş°Ñ‘S‘Ì‚ğ§Œä‚·‚é¸×½
class Game
{
private:
	// ¼İ¸ŞÙÄİ‚ÌŒˆ‚Ü‚è
	Game();							// ¶¬‹Ö~
	Game(const Game&);				// ºËß°‹Ö~
	void operator=(const Game&);	// ‘ã“ü‹Ö~

	const Vector2 ScreenSize;
	float time, fps, count, oldcount;
	Box field;

public:
	// ¼İ¸ŞÙÄİ²İÀ°Ìª°½
	static Game& Instance()
	{
		static Game instance;
		return instance;
	}
	~Game();

	// ‰Šú‰»Œn
	// Šî–{‘fŞ‚ÌÛ°ÄŞ‚Æ‚©‰Šúˆ—‚ğs‚¤
	void Initialize();

	// ’†‚ÉÙ°Ìß‚ ‚è
	// ‚±‚ê‚ğŒÄ‚Ô‚Æ¹Ş°Ñ‚ªn‚Ü‚é
	void Run();

	// Œãˆ—
	// ×²ÌŞ×Ø‚ÌŒãˆ—‚â
	void Terminate();

	// Œ»İÀs’†‚Ì¼°İ‚ÌŠÇ—
	std::shared_ptr<Scene> scene;

	// ¼°İ“ü‚ê‘Ö‚¦
	void ChangeScene(Scene* s);

	const Vector2& GetScreenSize()const;
	const Box& GetFieldSize()const;
};

