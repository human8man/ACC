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
	CEndUI(HWND hWnd);
	~CEndUI();

	void Create();	// �쐬����.
	void Init();			// ����������.
	void Update();			// �X�V����.
	void Draw();			// �`�揈��.
	void Release();			// �������.

	// End�V�[�����폜�\�����擾.
	bool GetDeleteFlag() const { return m_EndDeleteFlag; }
private:
	HWND m_hWnd;	// �E�B���h�E�n���h��.
	D3DXVECTOR2 m_WindowRect;	// �E�B���h�E�ʒu�ۑ��p.

	bool m_EndDeleteFlag;		// End�V�[���폜�t���O.
};
