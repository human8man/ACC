#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"


//=============================================================================
//		TitleUI�N���X.
//=============================================================================
class CTitleUI
	:public CUIObject
{
public:
	CTitleUI();
	~CTitleUI();

	void Create();		// �쐬����.
	HRESULT LoadData();	// �Ǎ�����.
	void Init();		// ����������.
	void Update();		// �X�V����.
	void Draw();		// �`�揈��.
	void Release();		// �������.

	// �Q�[���J�n�t���O�̎擾.
	bool GetStart() const { return m_Start; }

private:
	bool m_Start;	// �Q�[���J�n�t���O.
};
