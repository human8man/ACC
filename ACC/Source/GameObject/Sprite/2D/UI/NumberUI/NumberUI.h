#pragma once

#include "Sprite/2D/UI/UIObject.h"
#include "Time/Time.h"


class NumberUI
{
public:
	NumberUI();
	~NumberUI();

	void Create();	// �쐬����.
	void Update();	// �X�V����.
	void Draw();	// �`�揈��.

	/****************************************************************
	// @brief ������ݒ肷��.
	// @param pos	���W.
	// @param value	�l.
	// @param size	�傫��.
	****************************************************************/
	void SetNumber(D3DXVECTOR3 pos, int value, float size);

	// �F��ݒ�.
	void SetColor(D3DXVECTOR3 color);
	// ���ߐݒ�.
	void SetAlpha(float value);
	// ���F�ɔ���.
	void SetRainbow(bool flag) { m_Rainbow = flag; }
	void SetSpecial(bool flag) { m_Special = flag; }

	// �ݒ肳�ꂽ�l���擾.
	int GetValue()const { return m_SetValue; }
private:
	std::vector<UIObject*>	m_pNumber;
	Sprite2D* m_pSprite2D;
	D3DXVECTOR3 m_UIPos;	// �S�̂̊���W.
	std::vector<D3DXVECTOR3> m_NumberPos;	// ���ꂼ��̌����W.
	D3DXVECTOR3 m_Color;	// �S�̂̐F.

	int m_SetValue;
	int m_Digit;
	int m_Minus;
	bool m_Rainbow;
	bool m_Special;
};