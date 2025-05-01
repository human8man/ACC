#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/CSprite2D.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"

// 前方宣言.
class CUIObject;
class CStaticMesh;

//=============================================================================
//		タイトルシーンクラス.
//=============================================================================
class CTitle
	: public CSceneBase
{
public:
	CTitle(HWND hWnd);
	~CTitle();

	void Create() override;			// 作成処理.
	HRESULT LoadData() override;	// 読込処理.
	void Init() override;			// 初期化処理.
	void Update() override;			// 更新処理.
	void Draw() override;			// 描画処理.
	void Release() override;		// 解放処理.

private:
	LIGHT		m_Light;
	D3DXMATRIX	m_mView;
	D3DXMATRIX	m_mProj;

	// 画像情報リスト.
	std::vector<std::string> m_SpriteDataList;	// スプライト情報をまとめる配列.
	std::vector<D3DXVECTOR3> m_SpritePosList;	// スプライト座標をまとめる配列.

	std::vector<CUIObject*> m_pUIs;			// UIクラス.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2Dクラス.

	CStaticMesh* m_pEgg;	// 卵.

	bool m_Start;	// ゲーム開始フラグ.
};