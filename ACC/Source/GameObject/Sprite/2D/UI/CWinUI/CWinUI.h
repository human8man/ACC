#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"


//=============================================================================
//		WinUI�N���X.
//=============================================================================
class CWinUI
	:public CUIObject
{
public:
	// Title�Ŏg�p����X�v���C�g����ǉ����Ă���(���O��).
	enum WinSprite {
		Black,
		Victory
	};

public:
	CWinUI();
	~CWinUI();

	void Create();		// �쐬����.
	HRESULT LoadData();	// �Ǎ�����.
	void Init();		// ����������.
	void Update();		// �X�V����.
	void Draw();		// �`�揈��.
	void Release();		// �������.

private:
	float m_SpawnTimeMax;	// �X�|�[���ő厞��.
	float m_SpawnTime;		// �X�|�[������.
};