#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/CSprite2D.h"
#include "DirectInput/CDirectInput.h"

//�O���錾
class CUIObject;
class CStaticMesh;


//=============================================================================
//		�^�C�g���V�[���N���X.
//=============================================================================
class CTitle
	: public CSceneBase
{
public:
	// Title�Ŏg�p����X�v���C�g����ǉ����Ă���(���O��).
	enum TitleSprite {
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
	CTitle(HWND hWnd);
	~CTitle();

	void Create() override;			// �쐬����.
	HRESULT LoadData() override;	// �Ǎ�����.
	void Init() override;			// ����������.
	void Update() override;			// �X�V����.
	void Draw() override;			// �`�揈��.
	void Release() override;		// �������.

private:
	// �_�Ǝl�p�̓����蔻��(�X�v���C�g�p).
	bool PointInSquare(POINT ppos, D3DXVECTOR2 spos, D3DXVECTOR2 sposs);

private:
	LIGHT		m_Light;
	D3DXMATRIX	m_mView;
	D3DXMATRIX	m_mProj;

	// �摜��񃊃X�g.
	std::vector<std::string> m_SpriteDataList;//�X�v���C�g�����܂Ƃ߂�z��.

	std::vector<D3DXVECTOR3> m_SpritePosList;	//�X�v���C�g���W���܂Ƃ߂�z��.

	std::vector<CUIObject*> m_pUIs;			// UI�N���X.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2D�N���X.

	CStaticMesh* m_pEgg;	// ��.
};