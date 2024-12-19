#include "CUIObject.h"
#include "DirectX//CDirectX11.h"


CUIObject::CUIObject()
	: m_pSprite		( nullptr )
	, m_PatternNo	()
{
}

CUIObject::~CUIObject()
{
	DetachSprite();
}

void CUIObject::Update()
{
	if( m_pSprite == nullptr ){
		return;
	}
}

void CUIObject::Draw()
{
	if( m_pSprite == nullptr ){
		return;
	}

	//描画直前で座標や回転情報などを更新.
	m_pSprite->SetPosition( m_vPosition );
	m_pSprite->SetRotation( m_vRotation );
	m_pSprite->SetScale( m_vScale );
	m_pSprite->SetAlpha( m_Alpha );

	//パターン番号を設定.
	m_pSprite->SetPatternNo( m_PatternNo.x, m_PatternNo.y );


	CDirectX11::GetInstance()->SetDepth(false);
	//レンダリング.
	m_pSprite->Render();
	CDirectX11::GetInstance()->SetDepth(true);
}

void CUIObject::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj,
	LIGHT& Light )
{
	Draw();
}

