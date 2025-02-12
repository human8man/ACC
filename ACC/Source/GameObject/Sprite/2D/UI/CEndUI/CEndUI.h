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
	// End�V�[���Ŏg�p����X�v���C�g����ǉ�(���O��).
	enum EndSprite {
		Back,
		Frame,
		SelectNo,
		SelectYes,
	};

public:
	CEndUI();
	~CEndUI();

	void Create(HWND hWnd);	// �쐬����.
	void Init();			// ����������.

	void Update();			// �X�V����.
	void Draw();			// �`�揈��.

	void Release();			// �������.


	// End�V�[�����폜�\�����擾.
	bool GetDeleteFlag() const { return m_EndDeleteFlag; }

private:
	// �E�B���h�E�ʒu�̌v�Z���܂Ƃ߂��֐�.
	D3DXVECTOR2 WindowRectMath();

private:
	HWND m_hWnd;	// �E�B���h�E�n���h��.

	// �摜��񃊃X�g.
	std::vector<std::string> m_SpriteDataList;	// �X�v���C�g�����܂Ƃ߂�z��.
	std::vector<D3DXVECTOR3> m_SpritePosList;	// �X�v���C�g���W���܂Ƃ߂�z��.
	std::vector<CUIObject*> m_pUIs;				// UI�N���X.
	std::vector<CSprite2D*> m_pSprite2Ds;		// Sprite2D�N���X.

	D3DXVECTOR2 m_WindowRect;	// �E�B���h�E�ʒu�ۑ��p.

	bool m_EndDeleteFlag;		// End�V�[���폜�t���O.
};
