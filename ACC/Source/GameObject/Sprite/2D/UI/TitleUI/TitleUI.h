#pragma once

#include "Scenes/SceneBase.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectInput/DirectInput.h"

class NumberUI;

class TitleUI
	:public UIObject
{
public:
	TitleUI();
	~TitleUI();

	void Create();		// 作成処理
	HRESULT LoadData();	// 読込処理
	void Init();		// 初期化処理
	void Update();		// 更新処理
	void Draw();		// 描画処理
	void Release();		// 解放処理

	// ゲーム開始フラグの取得
	bool GetStart() const { return m_Start; }

private:
	std::unique_ptr<NumberUI>	m_pNumberUI;
	D3DXVECTOR3 m_NumPos;
	D3DXVECTOR3 m_Size;
	bool m_Start;	// ゲーム開始フラグ
};
