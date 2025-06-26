#pragma once

#include "GameObject.h"
#include "Sprite/3D/Sprite3D.h"


class SpriteObject
	: public GameObject
{
public:
	SpriteObject();
	virtual ~SpriteObject() override;

	// 更新処理
	virtual void Update() override;
	// 描画処理
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj );

	//スプライトを接続する
	void AttachSprite( Sprite3D& pSprite ){
		m_pSprite = &pSprite;
	}
	// スプライトを切り離す
	void DetachSprite(){
		m_pSprite = nullptr;
	}

protected:
	// 子クラスを抽象クラスにしないために必要
	void Draw(D3DXMATRIX& mView, D3DXMATRIX& mProj, LIGHT& Light ) override final;

protected:
	Sprite3D* m_pSprite;
};