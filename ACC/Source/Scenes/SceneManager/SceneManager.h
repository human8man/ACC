#pragma once

#include "Singleton/Singleton.h"

// 前方宣言
class FadeUI;
class EndUI;
class SceneBase;

// シーンリスト列挙型
enum SceneList {
	
	eUIEditor = -1,

	eTitle,
	eGame,
};


class SceneManager
	: public Singleton<SceneManager>
{
private:
	friend class Singleton<SceneManager>;

public:
	SceneManager();
	~SceneManager();

	static HRESULT Create(HWND hWnd);	// 作成処理
	void LoadScene(SceneList Scene);	// 読込処理
	void Update();						// 更新処理
	void Draw();						// 描画処理
	void Release();						// 解放処理

	static SceneList& GetSceneNo() { return GetInstance()->m_SceneNo; }

private:
	// 指定されたシーンの生成
	std::unique_ptr<SceneBase> CreateScene(SceneList No);
	// カーソルの表示切替
	void ChangeShowCursor(bool flag);
	// 終了処理
	void EndProcess();
	// 定数処理
	void ConstantProcess() const;
	// ImGui処理
	void ImGuiUpdate();
	// シーン更新処理
	void SceneUpdate();

private:
	HWND m_hWnd;
	SceneList m_SceneNo;		// 現在のシーン番号
	SceneList m_NextSceneNo;	// 次のシーン番号
	std::unique_ptr<FadeUI>	m_pFade;	// フェードクラス
	std::unique_ptr<EndUI>		m_pEndUI;	// エンドクラス
	std::unique_ptr<SceneBase>	m_pScene;	// シーンクラスのポインタ

	float m_FocusTime;		// ウィンドウにフォーカスする時間
	float m_FocusTimeMax;	// ウィンドウにフォーカスする最大時間
	bool m_EndDeleteFlag;
};