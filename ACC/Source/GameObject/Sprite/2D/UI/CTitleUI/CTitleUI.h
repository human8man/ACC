#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"


//=============================================================================
//		TitleUIクラス.
//=============================================================================
class CTitleUI
	:public CUIObject
{
public:
	CTitleUI();
	~CTitleUI();

	void Create();		// 作成処理.
	HRESULT LoadData();	// 読込処理.
	void Init();		// 初期化処理.
	void Update();		// 更新処理.
	void Draw();		// 描画処理.
	void Release();		// 解放処理.

	// ゲーム開始フラグの取得.
	bool GetStart() const { return m_Start; }

private:
	bool m_Start;	// ゲーム開始フラグ.
};
