#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectInput/CDirectInput.h"


class VictoryUI
	:public UIObject
{
public:
	// Title�Ŏg�p����X�v���C�g����ǉ����Ă���(���O��)
	enum WinSprite {
		Black,
		Victory
	};

public:
	VictoryUI();
	~VictoryUI();

	void Create();		// �쐬����
	HRESULT LoadData();	// �Ǎ�����
	void Init();		// ����������
	void Update();		// �X�V����
	void Draw();		// �`�揈��
	void Release();		// �������

private:
	float m_SpawnTimeMax;	// �X�|�[���ő厞��
	float m_SpawnTime;		// �X�|�[������
};