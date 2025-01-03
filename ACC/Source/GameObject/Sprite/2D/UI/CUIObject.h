#pragma once

#include "../GameObject/CGameObject.h"

#include "../CSprite2D.h"

/************************************************************
*	UI�I�u�W�F�N�g�N���X.
**/
class CUIObject
	: public CGameObject
{
public:
	CUIObject();
	virtual ~CUIObject() override;

	//CGameObject�ŏ������z�֐��̐錾������Ă�̂ł�����Œ�`������.
	virtual void Update() override;
	//CUIObject�Ő錾�����Ŋ֐��ŁA�ȍ~�͂����override������.
	virtual void Draw();

	//�X�v���C�g��ڑ�����.
	void AttachSprite( CSprite2D& pSprite ){
		m_pSprite = &pSprite;
	}
	//�X�v���C�g��؂藣��.
	void DetachSprite(){
		m_pSprite = nullptr;
	}
	//�p�^�[���ԍ���ݒ�.
	void SetPatternNo( SHORT x, SHORT y ){
		m_PatternNo.x = x;
		m_PatternNo.y = y;
	}


	POINTS GetPatternNo() const { return m_PatternNo; }
	CSprite2D::SPRITE_STATE GetSpriteData() const { return m_pSprite->GetSpriteData(); }

protected:
	//CGameObject����override�����֐�.
	//final ����ȍ~��override�����Ȃ�.
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj,
		LIGHT& Light ) override final;

protected:
	CSprite2D*	m_pSprite;
	POINTS		m_PatternNo;	//�p�^�[���ԍ�(�}�X��).
};
