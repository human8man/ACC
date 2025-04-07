#pragma once

#include "CGameObject.h"
#include "Sprite/3D/CSprite3D.h"


//==============================================================================
//		スプライトオブジェクトクラス.
//==============================================================================
class CSpriteObject
	: public CGameObject
{
public:
	CSpriteObject();
	virtual ~CSpriteObject() override;

	// 更新処理.
	virtual void Update() override;
	// 描画処理.
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj );

	//スプライトを接続する.
	void AttachSprite( CSprite3D& pSprite ){
		m_pSprite = &pSprite;
	}
	// スプライトを切り離す.
	void DetachSprite(){
		m_pSprite = nullptr;
	}

protected:
	// 子クラスを抽象クラスにしないために必要.
	void Draw(D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light ) override final;

protected:
	CSprite3D* m_pSprite;
};