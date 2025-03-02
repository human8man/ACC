#pragma once

#include "CGameObject.h"
#include "../CSprite2D.h"


//=============================================================================
//		UIオブジェクトクラス.
//=============================================================================
class CUIObject
	: public CGameObject
{
public:
	CUIObject();
	virtual ~CUIObject() override;

	// 更新処理.
	virtual void Update() override;
	// 描画処理.
	virtual void Draw();

	// スプライトを接続する.
	void AttachSprite( CSprite2D& pSprite ){ m_pSprite = &pSprite; }
	// スプライトを切り離す.
	void DetachSprite(){ m_pSprite = nullptr; }

	// パターン番号を設定.
	void SetPatternNo( SHORT x, SHORT y ){
		m_PatternNo.x = x;
		m_PatternNo.y = y;
	}

	// パターン番号を取得.
	POINTS GetPatternNo() const { return m_PatternNo; }
	// スプライト情報の取得.
	CSprite2D::SPRITE_STATE GetSpriteData() const { return m_pSprite->GetSpriteData(); }


	// 点と四角のあたり判定.
	bool PointInSquare(POINT ppos, D3DXVECTOR2 spos, D3DXVECTOR2 sposs);

protected:
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override final;

protected:
	CSprite2D*	m_pSprite;
	POINTS		m_PatternNo;	//パターン番号(マス目).
};
