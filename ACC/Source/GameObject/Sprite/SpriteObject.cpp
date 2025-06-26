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
//		更新処理
//==============================================================================
void SpriteObject::Update()
{
	if( m_pSprite == nullptr ){
		return;
	}
}


//==============================================================================
//		描画処理
//==============================================================================
void SpriteObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj )
{
	if( m_pSprite == nullptr ){ return; }

	// 描画直前で座標や回転情報などを更新
	m_pSprite->SetPosition( m_vPosition );
	m_pSprite->SetRotation( m_vRotation );
	m_pSprite->SetScale( m_vScale );

	// レンダリング
	m_pSprite->Render( View, Proj );
}


//-----------------------------------------------------------------------------
//		子クラスを抽象クラスにしないために必要
//-----------------------------------------------------------------------------
void SpriteObject::Draw(D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light)
{
	Draw(mView, mProj);
}