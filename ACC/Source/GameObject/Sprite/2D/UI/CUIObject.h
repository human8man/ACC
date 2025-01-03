#pragma once

#include "../GameObject/CGameObject.h"

#include "../CSprite2D.h"

/************************************************************
*	UIオブジェクトクラス.
**/
class CUIObject
	: public CGameObject
{
public:
	CUIObject();
	virtual ~CUIObject() override;

	//CGameObjectで純粋仮想関数の宣言がされてるのでこちらで定義を書く.
	virtual void Update() override;
	//CUIObjectで宣言したで関数で、以降はこれをoverrideさせる.
	virtual void Draw();

	//スプライトを接続する.
	void AttachSprite( CSprite2D& pSprite ){
		m_pSprite = &pSprite;
	}
	//スプライトを切り離す.
	void DetachSprite(){
		m_pSprite = nullptr;
	}
	//パターン番号を設定.
	void SetPatternNo( SHORT x, SHORT y ){
		m_PatternNo.x = x;
		m_PatternNo.y = y;
	}


	POINTS GetPatternNo() const { return m_PatternNo; }
	CSprite2D::SPRITE_STATE GetSpriteData() const { return m_pSprite->GetSpriteData(); }

protected:
	//CGameObjectからoverrideした関数.
	//final これ以降はoverrideさせない.
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj,
		LIGHT& Light ) override final;

protected:
	CSprite2D*	m_pSprite;
	POINTS		m_PatternNo;	//パターン番号(マス目).
};
