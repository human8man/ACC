#include "CSpriteObject.h"


//==============================================================================
//		スプライトオブジェクトクラス.
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
//		更新処理.
//==============================================================================
void CSpriteObject::Update()
{
	if( m_pSprite == nullptr ){
		return;
	}
}


//==============================================================================
//		描画処理.
//==============================================================================
void CSpriteObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj )
{
	if( m_pSprite == nullptr ){ return; }

	// 描画直前で座標や回転情報などを更新.
	m_pSprite->SetPosition( m_vPosition );
	m_pSprite->SetRotation( m_vRotation );
	m_pSprite->SetScale( m_vScale );

	// レンダリング.
	m_pSprite->Render( View, Proj );
}