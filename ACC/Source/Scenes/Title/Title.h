#pragma once

#include "Scenes/SceneBase.h"
#include "Sprite/2D/Sprite2D.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectInput/DirectInput.h"


class UIObject;
class StaticMesh;
class TitleUI;


class Title
	: public SceneBase
{
public:
	Title(HWND hWnd);
	~Title();

	void Create() override;			// 作成処理
	HRESULT LoadData() override;	// 読込処理
	void Init() override;			// 初期化処理
	void Update() override;			// 更新処理
	void Draw() override;			// 描画処理
	void Release() override;		// 解放処理
private:
	LIGHT		m_Light;
	D3DXMATRIX	m_mView;
	D3DXMATRIX	m_mProj;

	std::unique_ptr<TitleUI>	m_pTitleUI;
	StaticMesh* m_pEgg;	// 卵

	bool m_Start;	// ゲーム開始フラグ
};