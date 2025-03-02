#include "CSpriteObject.h"


//==============================================================================
//		�X�v���C�g�I�u�W�F�N�g�N���X.
//==============================================================================
CSpriteObject::CSpriteObject()
	: m_pSprite		( nullptr )
{
}

CSpriteObject::~CSpriteObject()
{
	DetachSprite();
}


//==============================================================================
//		�X�V����.
//==============================================================================
void CSpriteObject::Update()
{
	if( m_pSprite == nullptr ){
		return;
	}
}


//==============================================================================
//		�`�揈��.
//==============================================================================
void CSpriteObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj )
{
	if( m_pSprite == nullptr ){ return; }

	// �`�撼�O�ō��W���]���Ȃǂ��X�V.
	m_pSprite->SetPosition( m_vPosition );
	m_pSprite->SetRotation( m_vRotation );
	m_pSprite->SetScale( m_vScale );

	// �����_�����O.
	m_pSprite->Render( View, Proj );
}