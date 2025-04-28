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
//		スプライトを切り離す.
//=============================================================================
void CUIObject::DetachSprite()
{
	m_pSprite = nullptr;
}


//=============================================================================
//		点と四角のあたり判定.
//=============================================================================
bool CUIObject::PointInSquare( POINT ppos, D3DXVECTOR2 windowpos )
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


//=============================================================================
//		パターン番号を設定.
//=============================================================================
void CUIObject::SetPatternNo( SHORT x, SHORT y )
{
	m_PatternNo = POINTS(x,y);
}


//=============================================================================
//		画像の元サイズを設定.
//=============================================================================
void CUIObject::SetBase( D3DXVECTOR2& base )
{
	m_pSprite->SetBase(base);
}


//=============================================================================
//		画像の表示範囲を設定.
//=============================================================================
void CUIObject::SetDisp( D3DXVECTOR2& disp )
{
	m_pSprite->SetDisp(disp);
}


//=============================================================================
//		画像の分割サイズを設定.
//=============================================================================
void CUIObject::SetStride( D3DXVECTOR2& stride )
{
	m_pSprite->SetStride(stride);
}


//=============================================================================
//		画像名を設定.
//=============================================================================
void CUIObject::SetSpriteName( const std::string& name )
{
	m_pSprite->SetSpriteName(name);
}


//=============================================================================
//		パターン番号を取得.
//=============================================================================
POINTS CUIObject::GetPatternNo() const
{
	return m_PatternNo;
}


//=============================================================================
//		スプライト情報の取得.
//=============================================================================
CSprite2D::SPRITE_STATE CUIObject::GetSpriteData() const
{  
	return m_pSprite->GetSpriteData();
}
