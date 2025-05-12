#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/CSprite2D.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"

// �O���錾.
class CUIObject;
class CStaticMesh;
class CTitleUI;

//=============================================================================
//		�^�C�g���V�[���N���X.
//=============================================================================
class CTitle
	: public CSceneBase
{
public:
	CTitle(HWND hWnd);
	~CTitle();

	void Create() override;			// �쐬����.
	HRESULT LoadData() override;	// �Ǎ�����.
	void Init() override;			// ����������.
	void Update() override;			// �X�V����.
	void Draw() override;			// �`�揈��.
	void Release() override;		// �������.

private:
	LIGHT		m_Light;
	D3DXMATRIX	m_mView;
	D3DXMATRIX	m_mProj;

	std::unique_ptr<CTitleUI>	m_pTitleUI;	// UI�N���X.
	CStaticMesh* m_pEgg;	// ��.

	bool m_Start;	// �Q�[���J�n�t���O.
};