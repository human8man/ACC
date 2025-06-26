#pragma once

#include "CGameObject.h"
#include "Sprite/2D/CSprite2D.h"


class CUIObject
	: public CGameObject
{
public:
	CUIObject();
	virtual ~CUIObject() override;

	// �X�V����.
	virtual void Update() override;
	// �`�揈��.
	virtual void Draw();

	// �X�v���C�g��ڑ�����.
	void AttachSprite(CSprite2D* pSprite);
	// �X�v���C�g��؂藣��.
	void DetachSprite();

	// �p�^�[���ԍ���ݒ�.
	void SetPatternNo(SHORT x, SHORT y);

	// �摜�̌��T�C�Y��ݒ�.
	void SetBase(D3DXVECTOR2& base);
	// �摜�̕\���͈͂�ݒ�.
	void SetDisp(D3DXVECTOR2& disp);
	// �摜�̕����T�C�Y��ݒ�.
	void SetStride(D3DXVECTOR2& stride);
	// �摜����ݒ�.
	void SetSpriteName(const std::string& name);
	
	// �p�^�[���ԍ����擾.
	POINTS GetPatternNo() const;
	// �X�v���C�g���̎擾.
	CSprite2D::SPRITE_STATE GetSpriteData() const;

	// �_�Ǝl�p�̂����蔻��.
	bool PointInSquare(POINT ppos, D3DXVECTOR2 windowpos);

protected:
	// �`��֐�.
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override final;

	/****************************************************************
	// @brief Scene���Ƃ�Json��������擾.
	// @param name	Json�̃p�X.
	// @param uis	std::vector<CUIObject*>.
	****************************************************************/
	void LoadFromJson(
		const std::string& scenepath,
		std::vector<CUIObject*>& uis);

protected:
	CSprite2D*	m_pSprite;
	POINTS		m_PatternNo;	// �p�^�[���ԍ�(�}�X��).

	// �摜��񃊃X�g.
	std::vector<std::string> m_SpriteDataList;	// �X�v���C�g�����܂Ƃ߂�z��.
	std::vector<CUIObject*> m_pUIs;			// UI�N���X.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2D�N���X.
};