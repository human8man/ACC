#include "CUIObject.h"
#include "DirectX/CDirectX11.h"


//=============================================================================
//		UI�I�u�W�F�N�g�N���X.
//=============================================================================
CUIObject::CUIObject()
	: m_pSprite		( nullptr )
	, m_PatternNo	()
{
}
CUIObject::~CUIObject()
{
	DetachSprite();
}


//=============================================================================
//		�X�V����.
//=============================================================================
void CUIObject::Update()
{
	if( m_pSprite == nullptr ){ return; }
}


//=============================================================================
//		�`�揈��.
//=============================================================================
void CUIObject::Draw()
{
	if( m_pSprite == nullptr ){ return; }

	// �`�撼�O�ō��W���]���Ȃǂ��X�V.
	m_pSprite->SetPosition( m_vPosition );
	m_pSprite->SetRotation( m_vRotation );
	m_pSprite->SetScale( m_vScale );
	m_pSprite->SetAlpha( m_Alpha );

	// �p�^�[���ԍ���ݒ�.
	m_pSprite->SetPatternNo( m_PatternNo.x, m_PatternNo.y );


	CDirectX11::GetInstance()->SetDepth(false);
	// �����_�����O.
	m_pSprite->Render();
	CDirectX11::GetInstance()->SetDepth(true);
}


//=============================================================================
//		�_�Ǝl�p�̂����蔻��.
//=============================================================================
bool CUIObject::PointInSquare(POINT ppos, D3DXVECTOR2 spos, D3DXVECTOR2 sposs)
{
	if (spos.x < ppos.x
	&&  spos.y < ppos.y
	&&  ppos.x < spos.x + sposs.x
	&&  ppos.y < spos.y + sposs.y)
	{
		return true;
	}

	return false;
}


//=============================================================================
//		�`�揈��.
//=============================================================================
void CUIObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	Draw();
}