#pragma once

#include "Scenes/SceneBase.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectInput/DirectInput.h"

class NumberUI;

class TitleUI
	:public UIObject
{
public:
	TitleUI();
	~TitleUI();

	void Create();		// �쐬����
	HRESULT LoadData();	// �Ǎ�����
	void Init();		// ����������
	void Update();		// �X�V����
	void Draw();		// �`�揈��
	void Release();		// �������

	// �Q�[���J�n�t���O�̎擾
	bool GetStart() const { return m_Start; }

private:
	std::unique_ptr<NumberUI>	m_pNumberUI;
	D3DXVECTOR3 m_NumPos;
	D3DXVECTOR3 m_Size;
	bool m_Start;	// �Q�[���J�n�t���O
};
