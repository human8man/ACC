#pragma once

#include "Scenes/CSceneBase.h"
#include "..//GameObject/Sprite/2D/CSprite2D.h"
#include "DirectInput/CDirectInput.h"

//前方宣言
class CUIObject;
class CStaticMesh;

/********************************************************************************
*	タイトルシーンクラス.
**/
class CTitle
	: public CSceneBase
{
public:
	// Titleで使用するスプライト名を追加していく(名前順).
	enum TitleSprite {
		FullScreen,
		Select1,
		Select2,
		Select3,
		Select4,
		Select5,
		StartButton,
		Training,
	};

public:
	CTitle(HWND hWnd);
	~CTitle();

	void Create() override;
	HRESULT LoadData() override;
	void Release() override;
	void Init() override;

	void Update() override;
	void Draw() override;

private:

	bool PointInSquare(POINT ppos, D3DXVECTOR2 spos, D3DXVECTOR2 sposs);

private:
	HWND		m_hwnd;
	LIGHT		m_Light;
	D3DXMATRIX	m_mView;
	D3DXMATRIX	m_mProj;

	// 画像情報リスト.
	std::vector<std::string> m_SpriteDataList;//スプライト情報をまとめる配列.

	std::vector<D3DXVECTOR3> m_SpritePosList;	//スプライト座標をまとめる配列.

	std::vector<CUIObject*> m_pUIs;			// UIクラス.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2Dクラス.

	CStaticMesh* m_pEgg;	// 卵.
};