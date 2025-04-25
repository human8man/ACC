#include "CUIObject.h"
#include "DirectX/CDirectX11.h"


//=============================================================================
//		UIオブジェクトクラス.
//=============================================================================
CUIObject::CUIObject()
	: m_pSprite			( nullptr )
	, m_PatternNo		()
{
}
CUIObject::~CUIObject()
{
	DetachSprite();
}


//=============================================================================
//		更新処理.
//=============================================================================
void CUIObject::Update()
{
	if( m_pSprite == nullptr ){ return; }
}


//=============================================================================
//		描画処理.
//=============================================================================
void CUIObject::Draw()
{
	if( m_pSprite == nullptr ){ return; }

	// 描画直前で座標や回転情報などを更新.
	m_pSprite->SetPosition( m_vPosition );
	m_pSprite->SetRotation( m_vRotation );
	m_pSprite->SetScale( m_vScale );
	m_pSprite->SetAlpha( m_Alpha );

	// パターン番号を設定.
	m_pSprite->SetPatternNo( m_PatternNo.x, m_PatternNo.y );


	CDirectX11::GetInstance()->SetDepth(false);
	// レンダリング.
	m_pSprite->Render();
	CDirectX11::GetInstance()->SetDepth(true);
}


//=============================================================================
//		スプライトを接続する.
//=============================================================================
void CUIObject::AttachSprite(CSprite2D& pSprite)
{
	m_pSprite = &pSprite;
	m_vRotation = m_pSprite->GetRotation();
	m_vScale = m_pSprite->GetScale();
	m_Alpha = m_pSprite->GetAlpha();
}




//=============================================================================
//		点と四角のあたり判定.
//=============================================================================
bool CUIObject::PointInSquare(POINT ppos, D3DXVECTOR2 windowpos)
{
	D3DXVECTOR3 pos = {
		m_vPosition.x * FULLSCREENSCALEX + windowpos.x,
		m_vPosition.y * FULLSCREENSCALEY + windowpos.y,
		0.f};

 	if (pos.x < ppos.x
	&&  pos.y < ppos.y
	&& ppos.x < pos.x + m_pSprite->GetSpriteData().Disp.w * m_vScale.x * FULLSCREENSCALEX 
	&& ppos.y < pos.y + m_pSprite->GetSpriteData().Disp.h * m_vScale.y * FULLSCREENSCALEY )
	{
		return true;
	}
	return false;
}


//=============================================================================
//		描画処理.
//=============================================================================
void CUIObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	Draw();
}