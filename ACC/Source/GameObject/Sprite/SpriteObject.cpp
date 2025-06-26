#include "SpriteObject.h"


SpriteObject::SpriteObject()
	: m_pSprite		( nullptr )
{
}
SpriteObject::~SpriteObject()
{
	DetachSprite();
}


//==============================================================================
//		�X�V����
//==============================================================================
void SpriteObject::Update()
{
	if( m_pSprite == nullptr ){
		return;
	}
}


//==============================================================================
//		�`�揈��
//==============================================================================
void SpriteObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj )
{
	if( m_pSprite == nullptr ){ return; }

	// �`�撼�O�ō��W���]���Ȃǂ��X�V
	m_pSprite->SetPosition( m_vPosition );
	m_pSprite->SetRotation( m_vRotation );
	m_pSprite->SetScale( m_vScale );

	// �����_�����O
	m_pSprite->Render( View, Proj );
}


//-----------------------------------------------------------------------------
//		�q�N���X�𒊏ۃN���X�ɂ��Ȃ����߂ɕK�v
//-----------------------------------------------------------------------------
void SpriteObject::Draw(D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light)
{
	Draw(mView, mProj);
}