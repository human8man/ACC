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


//===================================================
//		ウィンドウの描画開始位置を取得.
//===================================================
D3DXVECTOR2 CUIObject::WindowRect(HWND hwnd)
{
	// ウィンドウ全体の位置とサイズを取得（ウィンドウタブや枠を含む）.
	RECT WindowRect;
	GetWindowRect(hwnd, &WindowRect);

	// クライアント領域の位置とサイズを取得（ウィンドウ内の描画範囲）.
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	// クライアント領域の左上と右下の座標を初期化.
	POINT topLeft = { clientRect.left, clientRect.top };
	POINT bottomRight = { clientRect.right, clientRect.bottom };

	// クライアント領域の座標をスクリーン座標系に変換.
	ClientToScreen(hwnd, &topLeft);
	ClientToScreen(hwnd, &bottomRight);

	// ウィンドウ全体の左上座標とクライアント領域の左上座標の差分を計算.
	int borderLeft	= topLeft.x - WindowRect.left;
	int borderTop	= topLeft.y - WindowRect.top;

	// フレーム幅を含んだウィンドウの位置を返す.
	return D3DXVECTOR2(
		static_cast<float>(borderLeft + WindowRect.left),
		static_cast<float>(borderTop + WindowRect.top));
}


//=============================================================================
//		点と四角のあたり判定.
//=============================================================================
bool CUIObject::PointInSquare(POINT ppos, D3DXVECTOR2 windowpos)
{
	D3DXVECTOR3 pos = {
		m_vPosition.x * FULLSCREENSCALE + windowpos.x,
		m_vPosition.y * FULLSCREENSCALE + windowpos.y,
		m_vPosition.z * FULLSCREENSCALE };

 	if (pos.x < ppos.x
	&&  pos.y < ppos.y
	&& ppos.x < pos.x + m_pSprite->GetSpriteData().Disp.w * m_vScale.x * FULLSCREENSCALE 
	&& ppos.y < pos.y + m_pSprite->GetSpriteData().Disp.h * m_vScale.y * FULLSCREENSCALE )
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