#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/CSprite2D.h"
#include "DirectInput/CDirectInput.h"

//�O���錾
class CUIObject;

//=============================================================================
//		WinUI�N���X.
//=============================================================================
class CWinUI
{
public:
	// Title�Ŏg�p����X�v���C�g����ǉ����Ă���(���O��).
	enum WinSprite {
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
	CWinUI(HWND hWnd);
	~CWinUI();

	void Create();
	HRESULT LoadData();
	void Release();
	void Init();

	void Update();
	void Draw();

private:
	// ��`�Ɠ_�̔���.
	bool PointInSquare(POINT ppos, D3DXVECTOR2 spos, D3DXVECTOR2 sposs);

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
};
