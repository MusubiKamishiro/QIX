#pragma once

// 周辺機器(ｷｰﾎﾞｰﾄﾞ&ﾊﾟｯﾄﾞ)の入力を制御
class Peripheral
{
private:
	int padState;		// 現在の入力情報
	int lastPadState;	// 直前ﾌﾚｰﾑの入力状態
public:
	Peripheral();
	~Peripheral();

	// 入力情報の更新(毎ﾌﾚｰﾑ)
	void Update();

	// 現在の押下状態の検出
	// @param keyid　調べたいｷｰ番号
	// @pretval true 押してる, false 押してない
	bool IsPressing(int keyid)const;

	// 現在のﾄﾘｶﾞｰ状態(押した瞬間)の検出
	bool IsTrigger(int keyid)const;
};

