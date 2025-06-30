#pragma once

#include "Sprite/2D/UI/UIObject.h"
#include "Time/Time.h"


class NumberUI
{
public:
	NumberUI();
	~NumberUI();

	void Create();	// 作成処理.
	void Update();	// 更新処理.
	void Draw();	// 描画処理.

	/****************************************************************
	// @brief 数字を設定する.
	// @param pos	座標.
	// @param value	値.
	// @param size	大きさ.
	****************************************************************/
	void SetNumber(D3DXVECTOR3 pos, int value, float size);

	// 色を設定.
	void SetColor(D3DXVECTOR3 color);
	// 透過設定.
	void SetAlpha(float value);
	// 虹色に発光.
	void SetRainbow(bool flag) { m_Rainbow = flag; }
	void SetSpecial(bool flag) { m_Special = flag; }

	// 設定された値を取得.
	int GetValue()const { return m_SetValue; }
private:
	std::vector<UIObject*>	m_pNumber;
	Sprite2D* m_pSprite2D;
	D3DXVECTOR3 m_UIPos;	// 全体の基準座標.
	std::vector<D3DXVECTOR3> m_NumberPos;	// それぞれの元座標.
	D3DXVECTOR3 m_Color;	// 全体の色.

	int m_SetValue;
	int m_Digit;
	int m_Minus;
	bool m_Rainbow;
	bool m_Special;
};