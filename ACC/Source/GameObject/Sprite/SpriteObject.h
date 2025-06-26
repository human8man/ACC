#pragma once

#include "GameObject.h"
#include "Sprite/3D/Sprite3D.h"


class SpriteObject
	: public GameObject
{
public:
	SpriteObject();
	virtual ~SpriteObject() override;

	// �X�V����
	virtual void Update() override;
	// �`�揈��
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj );

	//�X�v���C�g��ڑ�����
	void AttachSprite( Sprite3D& pSprite ){
		m_pSprite = &pSprite;
	}
	// �X�v���C�g��؂藣��
	void DetachSprite(){
		m_pSprite = nullptr;
	}

protected:
	// �q�N���X�𒊏ۃN���X�ɂ��Ȃ����߂ɕK�v
	void Draw(D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light ) override final;

protected:
	Sprite3D* m_pSprite;
};