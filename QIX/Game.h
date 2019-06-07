#pragma once
#include <memory>
#include "Scene/Scene.h"
#include "Geometry.h"


// �ްёS�̂𐧌䂷��׽
class Game
{
private:
	// �ݸ���݂̌��܂�
	Game();							// �����֎~
	Game(const Game&);				// ��߰�֎~
	void operator=(const Game&);	// ����֎~

	const Vector2 ScreenSize;
	float time, fps, count, oldcount;
	Box field;

public:
	// �ݸ���ݲ���̪��
	static Game& Instance()
	{
		static Game instance;
		return instance;
	}
	~Game();

	// �������n
	// ��{�f�ނ�۰�ނƂ������������s��
	void Initialize();

	// ����ٰ�߂���
	// ������ĂԂƹްт��n�܂�
	void Run();

	// �㏈��
	// ײ���؂̌㏈����
	void Terminate();

	// ���ݎ��s���̼�݂̊Ǘ�
	std::shared_ptr<Scene> scene;

	// ��ݓ���ւ�
	void ChangeScene(Scene* s);

	const Vector2& GetScreenSize()const;
	const Box& GetFieldSize()const;
};

