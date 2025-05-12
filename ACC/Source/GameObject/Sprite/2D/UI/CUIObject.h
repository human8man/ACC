#pragma once

#include "CGameObject.h"
#include "Sprite/2D/CSprite2D.h"


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
	void AttachSprite(CSprite2D* pSprite);
	// スプライトを切り離す.
	void DetachSprite();

	// パターン番号を設定.
	void SetPatternNo(SHORT x, SHORT y);

	// 画像の元サイズを設定.
	void SetBase(D3DXVECTOR2& base);
	// 画像の表示範囲を設定.
	void SetDisp(D3DXVECTOR2& disp);
	// 画像の分割サイズを設定.
	void SetStride(D3DXVECTOR2& stride);
	// 画像名を設定.
	void SetSpriteName(const std::string& name);
	
	// パターン番号を取得.
	POINTS GetPatternNo() const;
	// スプライト情報の取得.
	CSprite2D::SPRITE_STATE GetSpriteData() const;

	// 点と四角のあたり判定.
	bool PointInSquare(POINT ppos, D3DXVECTOR2 windowpos);

protected:
	// 描画関数.
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override final;

	/****************************************************************
	// @brief SpiteManagerから画像名のリストと同じ名前のデータを読込.
	// @param name 画像名リスト("Data/Texture"内のSprite名参照).
	// @param uis 受取のUIObject配列.
	// @param sprites 受取のCSprite2D配列.
	****************************************************************/
	void LoadSpriteList(
		const std::vector<std::string>& name,
		std::vector<CUIObject*> &uis,
		std::vector<CSprite2D*> &sprites);
protected:
	CSprite2D*	m_pSprite;
	POINTS		m_PatternNo;	// パターン番号(マス目).

	// 画像情報リスト.
	std::vector<std::string> m_SpriteDataList;	// スプライト情報をまとめる配列.
	std::vector<CUIObject*> m_pUIs;			// UIクラス.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2Dクラス.
};