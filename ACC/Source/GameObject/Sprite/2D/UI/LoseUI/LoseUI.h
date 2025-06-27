#pragma once

#include "Scenes/SceneBase.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectInput/DirectInput.h"


class LoseUI
	:public UIObject
{
public:
	LoseUI();
	~LoseUI();

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
