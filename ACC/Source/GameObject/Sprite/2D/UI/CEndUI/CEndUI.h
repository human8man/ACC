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
	// Endで使用するスプライト名を追加していく(名前順).
	enum EndSprite {
		Back,
		Frame,
		SelectNo,
		SelectYes,
	};

public:
	CEndUI();
	~CEndUI();

	void Create(HWND hWnd);
	void Release();
	void Init();

	void Update();
	void Draw();

	bool GetDeleteFlag() const { return m_EndDeleteFlag; }

private:
	HWND		m_hWnd;
	LIGHT		m_Light;
	D3DXMATRIX	m_mView;
	D3DXMATRIX	m_mProj;

	// 画像情報リスト.
	std::vector<std::string> m_SpriteDataList;	// スプライト情報をまとめる配列.
	std::vector<D3DXVECTOR3> m_SpritePosList;	// スプライト座標をまとめる配列.

	std::vector<CUIObject*> m_pUIs;			// UIクラス.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2Dクラス.

	bool m_EndDeleteFlag;
};
