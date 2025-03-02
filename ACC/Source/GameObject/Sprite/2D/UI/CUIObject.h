#pragma once

#include "CGameObject.h"
#include "../CSprite2D.h"


//=============================================================================
//		UI�I�u�W�F�N�g�N���X.
//=============================================================================
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
	void AttachSprite( CSprite2D& pSprite ){ m_pSprite = &pSprite; }
	// �X�v���C�g��؂藣��.
	void DetachSprite(){ m_pSprite = nullptr; }

	// �p�^�[���ԍ���ݒ�.
	void SetPatternNo( SHORT x, SHORT y ){
		m_PatternNo.x = x;
		m_PatternNo.y = y;
	}

	// �p�^�[���ԍ����擾.
	POINTS GetPatternNo() const { return m_PatternNo; }
	// �X�v���C�g���̎擾.
	CSprite2D::SPRITE_STATE GetSpriteData() const { return m_pSprite->GetSpriteData(); }


	// �_�Ǝl�p�̂����蔻��.
	bool PointInSquare(POINT ppos, D3DXVECTOR2 spos, D3DXVECTOR2 sposs);

protected:
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override final;

protected:
	CSprite2D*	m_pSprite;
	POINTS		m_PatternNo;	//�p�^�[���ԍ�(�}�X��).
};
