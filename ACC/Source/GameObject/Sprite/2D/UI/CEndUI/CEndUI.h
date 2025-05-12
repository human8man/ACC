#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"


//=============================================================================
//		EndUIクラス.
//=============================================================================
class CEndUI
	:public CUIObject
{
public:
	CEndUI(HWND hWnd);
	~CEndUI();

	void Create();	// 作成処理.
	void Init();			// 初期化処理.
	void Update();			// 更新処理.
	void Draw();			// 描画処理.
	void Release();			// 解放処理.

	// Endシーンが削除可能かを取得.
	bool GetDeleteFlag() const { return m_EndDeleteFlag; }
private:
	HWND m_hWnd;	// ウィンドウハンドル.
	D3DXVECTOR2 m_WindowRect;	// ウィンドウ位置保存用.

	bool m_EndDeleteFlag;		// Endシーン削除フラグ.
};
