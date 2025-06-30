#pragma once

#include "Scenes/SceneBase.h"
#include "Sprite/2D/Sprite2D.h"
#include "Sprite/2D/UI/UIObject.h"
#include "DirectInput/DirectInput.h"


class UIObject;
class StaticMesh;
class TitleUI;


class Title
	: public SceneBase
{
public:
	Title(HWND hWnd);
	~Title();

	void Create() override;			// �쐬����
	HRESULT LoadData() override;	// �Ǎ�����
	void Init() override;			// ����������
	void Update() override;			// �X�V����
	void Draw() override;			// �`�揈��
	void Release() override;		// �������
private:
	LIGHT		m_Light;
	D3DXMATRIX	m_mView;
	D3DXMATRIX	m_mProj;

	std::unique_ptr<TitleUI>	m_pTitleUI;
	StaticMesh* m_pEgg;	// ��

	bool m_Start;	// �Q�[���J�n�t���O
};