#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"


//=============================================================================
//		WinUIクラス.
//=============================================================================
class CWinUI
	:public CUIObject
{
public:
	// Titleで使用するスプライト名を追加していく(名前順).
	enum WinSprite {
		Black,
		Victory
	};

public:
	CWinUI();
	~CWinUI();

	void Create();		// 作成処理.
	HRESULT LoadData();	// 読込処理.
	void Init();		// 初期化処理.
	void Update();		// 更新処理.
	void Draw();		// 描画処理.
	void Release();		// 解放処理.

private:
	float m_SpawnTimeMax;	// スポーン最大時間.
	float m_SpawnTime;		// スポーン時間.
};