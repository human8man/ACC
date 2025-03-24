#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"


//=============================================================================
//		WinUIクラス.
//=============================================================================
class CUIEditor
	: public CSceneBase
{
private:
	// 変更可能なUIのリスト.
	enum UISceneList {
		Title,
		Game,
		Lose,
		Win,
	};
public:
	CUIEditor(HWND hWnd);
	~CUIEditor();

	void Create()	override;	// 作成処理.
	HRESULT LoadData()override;	// データ読込.
	void Init()		override;	// 初期化処理.
	void Update()	override;	// 更新処理.
	void Draw()		override;	// 描画処理.
	void Release()	override;	// 解放処理.


private:
	// 選択したシーンのUIを読み込み.
	void SelectSceneLoad(UISceneList scene);
	// 現在シーンのUI情報を保存.
	HRESULT SaveScene();

private:
	// 画像情報リスト.
	std::vector<std::string> m_SpriteDataList;	//スプライト情報をまとめる配列.
	std::vector<D3DXVECTOR3> m_SpritePosList;	//スプライト座標をまとめる配列.

	std::vector<CUIObject*> m_pUIs;				// UIクラス.
	std::vector<CSprite2D*> m_pSprite2Ds;		// Sprite2Dクラス.

	std::string m_ScenePath;	// 現在のシーンパス.
	D3DXVECTOR2 m_OffsetPos;	// マウス座標と画像座標のズレ補正値.

	bool m_MovedSomething;		// 何か変更があった際に保存確認フラグを立てる.
};