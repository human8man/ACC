#include "CExplosion.h"


//=============================================================================
//		3dSprite�����N���X.
//=============================================================================
CExplosion::CExplosion()
	: m_PatternNo	()
	, m_AnimCount	()
{
}

CExplosion::~CExplosion()
{
}


//=============================================================================
//		�X�V����.
//=============================================================================
void CExplosion::Update()
{
	const POINTS PatternMax = m_pSprite->GetPatternMax();
	
	// �A�j���[�V�������Đ�����.
	m_AnimCount++;
	if (m_AnimCount % 10 == 0) {
		m_PatternNo.x = ( m_AnimCount / 10 ) % PatternMax.x;
		m_PatternNo.y = ( m_AnimCount / 10 ) / PatternMax.y;
	}
	if ( ( m_AnimCount / 10 ) >= PatternMax.x * PatternMax.y )
	{
		m_AnimCount = 0;
	}
}


//=============================================================================
//		�`�揈��.
//=============================================================================
void CExplosion::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj )
{
	// �摜�̃p�^�[���ݒ�.
	m_pSprite->SetPatternNo( m_PatternNo.x, m_PatternNo.y );

	// �r���{�[�h�ݒ�.
	m_pSprite->SetBillboard( true );

	// �`��.
	CSpriteObject::Draw( View, Proj );

	// �r���{�[�h�ݒ�.
	m_pSprite->SetBillboard( false );
}
