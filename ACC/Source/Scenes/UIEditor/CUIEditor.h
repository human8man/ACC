#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"


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
	// UI選択時に仮変数等を初期化する.
	void SelectInit();
	// 現在シーンのUI情報を保存.
	HRESULT SaveScene();

	//-------------------------------------------
	//	ImGuiを用いている関数.
	//-------------------------------------------
	// シーン選択関数.
	void ImGuiSelectScene();
	// UIリスト検索関数.
	void ImGuiSearchUI();
	// 座標調整関数(選択されたUIObect).
	void ImGuiPosEdit(CUIObject* object);
	// Z座標を元にソートする関数.
	void SortBySpritePosZ(CUIObject* object);
	// 情報調整関数(選択されたUIObect).
	void ImGuiInfoEdit(CUIObject* object);
	// パターンお試し関数(選択されたUIObect).
	void ImGuiPatternTest(CUIObject* object);
	// その他の情報調整関数(選択されたUIObect).
	void ImGuiEtcInfoEdit(CUIObject* object);

private:
	// 画像情報リスト.
	std::vector<std::string> m_SpriteDataList;	//スプライト情報をまとめる配列.
	std::vector<D3DXVECTOR3> m_SpritePosList;	//スプライト座標をまとめる配列.

	std::vector<CUIObject*> m_pUIs;				// UIクラス.
	std::vector<CSprite2D*> m_pSprite2Ds;		// Sprite2Dクラス.

	std::string m_ScenePath;	// 現在のシーンパス.

	bool	m_MovedSomething;	// 何か変更があった際に保存確認フラグを立てる.
	int		m_SelectedUIIndex;
	char	m_SearchBuffer[64] = ""; // 検索用バッファ.
	
	// マウス操作用の変数.
	D3DXVECTOR2 m_OffsetPos;	// マウス座標と画像座標のズレ補正値.
	bool m_DoDrag;				// ドラッグ中.
	bool m_MovedSpritePos;		// 画像座標が変更された際のフラグ.

	// パターン確認用の変数.
	POINTS	m_PatternNo;		// 仮のパターン.
	POINTS	m_PatternMax;		// 仮の最大パターン.
	bool	m_PatternAuto;		// パターンを自動で進める.
	float	m_AnimationSpeed;	// 送り速度(フレーム).
	float	m_AnimationSpeedMax;// 最大送り速度(フレーム).
};