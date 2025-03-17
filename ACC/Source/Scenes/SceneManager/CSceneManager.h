#pragma once

#include "Singleton/CSingleton.h"

// 前方宣言.
class CUIFade;
class CEndUI;
class CSceneBase;

// シーンリスト列挙型.
enum SceneList {
	
	UIEditor = -1,

	Title,
	Game,
};


//===============================================
//		シーンマネージャークラス.
//===============================================
class CSceneManager
	: public CSingleton<CSceneManager>
{
private:
	friend class CSingleton<CSceneManager>;

public:
	CSceneManager();
	~CSceneManager();

	static HRESULT Create(HWND hWnd);	// 作成処理.
	void LoadScene(SceneList Scene);	// 読込処理
	void Update();						// 更新処理.
	void Draw();						// 描画処理.
	void Release();						// 解放処理.

	static SceneList& GetSceneNo() { return GetInstance()->m_SceneNo; }
private:
	// 指定されたシーンの生成.
	std::unique_ptr<CSceneBase> CreateScene(SceneList No);

private:
	HWND m_hWnd;
	SceneList m_SceneNo;		// 現在のシーン番号.
	SceneList m_NextSceneNo;	// 次のシーン番号.
	std::unique_ptr<CUIFade>	m_pFade;	// フェードクラス.
	std::unique_ptr<CEndUI>		m_pEndUI;	// エンドクラス.
	std::unique_ptr<CSceneBase>	m_pScene;	// シーンクラスのポインタ.

	bool m_EndDeleteFlag;
};