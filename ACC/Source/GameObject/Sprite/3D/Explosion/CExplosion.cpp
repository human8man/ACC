#include "CExplosion.h"


//=============================================================================
//		3dSprite爆発クラス.
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
//		更新処理.
//=============================================================================
void CExplosion::Update()
{
	const POINTS PatternMax = m_pSprite->GetPatternMax();
	
	// アニメーションを再生する.
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
//		描画処理.
//=============================================================================
void CExplosion::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj )
{
	// 画像のパターン設定.
	m_pSprite->SetPatternNo( m_PatternNo.x, m_PatternNo.y );

	// ビルボード設定.
	m_pSprite->SetBillboard( true );

	// 描画.
	CSpriteObject::Draw( View, Proj );

	// ビルボード設定.
	m_pSprite->SetBillboard( false );
}
