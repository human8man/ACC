#pragma once

#include "CGameObject.h"
#include "Sprite/3D/CSprite3D.h"


//==============================================================================
//		�X�v���C�g�I�u�W�F�N�g�N���X.
//==============================================================================
class CSpriteObject
	: public CGameObject
{
public:
	CSpriteObject();
	virtual ~CSpriteObject() override;

	// �X�V����.
	virtual void Update() override;
	// �`�揈��.
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj );

	//�X�v���C�g��ڑ�����.
	void AttachSprite( CSprite3D& pSprite ){
		m_pSprite = &pSprite;
	}
	// �X�v���C�g��؂藣��.
	void DetachSprite(){
		m_pSprite = nullptr;
	}

protected:
	// �q�N���X�𒊏ۃN���X�ɂ��Ȃ����߂ɕK�v.
	void Draw(D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light ) override final;

protected:
	CSprite3D* m_pSprite;
};