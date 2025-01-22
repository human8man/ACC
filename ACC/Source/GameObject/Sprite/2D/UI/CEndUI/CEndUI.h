#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"


//=============================================================================
//		EndUI�N���X.
//=============================================================================
class CEndUI
	:public CUIObject
{
public:
	// End�Ŏg�p����X�v���C�g����ǉ����Ă���(���O��).
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

	// �摜��񃊃X�g.
	std::vector<std::string> m_SpriteDataList;	// �X�v���C�g�����܂Ƃ߂�z��.
	std::vector<D3DXVECTOR3> m_SpritePosList;	// �X�v���C�g���W���܂Ƃ߂�z��.

	std::vector<CUIObject*> m_pUIs;			// UI�N���X.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2D�N���X.

	bool m_EndDeleteFlag;
};
