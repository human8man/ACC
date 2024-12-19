#pragma once

#include "Singleton/CSingleton.h"

// 前方宣言.
class CUIFade;
class CSceneBase;

// シーンリスト列挙型.
enum SceneList {
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

	static HRESULT Create(HWND hWnd);
	void LoadScene(SceneList Scene);
	void Update();
	void Draw();
	void Release();

	static SceneList& GetSceneNo() { return GetInstance()->m_SceneNo; }
private:
	// 指定されたシーンの生成.
	std::unique_ptr<CSceneBase> CreateScene(SceneList No);

private:
	HWND m_hWnd;
	SceneList m_SceneNo;
	SceneList m_NextSceneNo;
	std::unique_ptr<CUIFade>	m_pFade;	// フェードクラス.
	std::unique_ptr<CSceneBase>	m_pScene;	// シーンクラスのポインタ.
};