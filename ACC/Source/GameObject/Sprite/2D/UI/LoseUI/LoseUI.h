#pragma once

#include "Scenes/SceneBase.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectInput/DirectInput.h"


class LoseUI
	:public UIObject
{
public:
	LoseUI();
	~LoseUI();

	void Create();		// 作成処理
	HRESULT LoadData();	// 読込処理
	void Init();		// 初期化処理
	void Update();		// 更新処理
	void Draw();		// 描画処理
	void Release();		// 解放処理

private:
	float m_SpawnTimeMax;	// スポーン最大時間
	float m_SpawnTime;		// スポーン時間
};
