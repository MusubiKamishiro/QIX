#pragma once

// ���Ӌ@��(���ް��&�߯��)�̓��͂𐧌�
class Peripheral
{
private:
	int padState;		// ���݂̓��͏��
	int lastPadState;	// ���O�ڰт̓��͏��
public:
	Peripheral();
	~Peripheral();

	// ���͏��̍X�V(���ڰ�)
	void Update();

	// ���݂̉�����Ԃ̌��o
	// @param keyid�@���ׂ������ԍ�
	// @pretval true �����Ă�, false �����ĂȂ�
	bool IsPressing(int keyid)const;

	// ���݂��ضް���(�������u��)�̌��o
	bool IsTrigger(int keyid)const;
};

