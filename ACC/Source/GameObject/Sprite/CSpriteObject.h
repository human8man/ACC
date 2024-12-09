#pragma once

#include "GameObject/CGameObject.h"

#include "GameObject/Sprite/3D/CSprite3D.h"

/************************************************************
*	スプライトオブジェクトクラス.
**/
class CSpriteObject
	: public CGameObject
{
public:
	CSpriteObject();
	virtual ~CSpriteObject() override;

	//CGameObjectで純粋仮想関数の宣言がされてるのでこちらで定義を書く.
	virtual void Update() override;
	//CSpriteObjectで宣言した関数で、以降はこれをoverrideさせる.
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj );

	//スプライトを接続する.
	void AttachSprite( CSprite3D& pSprite ){
		m_pSprite = &pSprite;
	}
	//スプライトを切り離す.
	void DetachSprite(){
		m_pSprite = nullptr;
	}

protected:
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj,
		LIGHT& Light, CAMERA& Camera ) override final;

protected:
	CSprite3D*	m_pSprite;

};
