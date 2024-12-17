#pragma once

#include "Singleton/CSingleton.h"

// 前方宣言.
class CTime;
class CUIFade;
class CSceneBase;

// シーンリスト列挙型.
enum SceneList {
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

private:
	// 指定されたシーンの生成.
	std::unique_ptr<CSceneBase> CreateScene(SceneList No);

private:
	HWND m_hWnd;
	SceneList m_NextSceneNo;
	std::unique_ptr<CUIFade>	m_pFade;	// フェードクラス.
	std::unique_ptr<CSceneBase>	m_pScene;	// シーンクラスのポインタ.
};