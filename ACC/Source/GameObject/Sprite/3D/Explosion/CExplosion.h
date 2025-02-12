#pragma once

#include "Sprite/CSpriteObject.h"


//=============================================================================
//		3dSprite�����N���X.
//=============================================================================
class CExplosion
	: public CSpriteObject	// �X�v���C�g�I�u�W�F�N�g�N���X���p��.
{
public:
	CExplosion();
	virtual ~CExplosion() override;


	// �X�V����.
	virtual void Update() override;
	// �`�揈��.
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj ) override;

	// �A�j���[�V�����̃��Z�b�g.
	void ResetAnimation(){
		m_PatternNo.x = 0;
		m_PatternNo.y = 0;
		m_AnimCount = 0;
	}

protected:
	POINTS	m_PatternNo;	// �p�^�[���ԍ�(�}�X��).
	int		m_AnimCount;	// �A�j���[�V�����J�E���g.
};
