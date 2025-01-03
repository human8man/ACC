#pragma once

#include "Scenes/CSceneBase.h"
#include "..//GameObject/Sprite/2D/CSprite2D.h"
#include "DirectInput/CDirectInput.h"

//�O���錾
class CUIObject;

/********************************************************************************
*	�^�C�g���V�[���N���X.
**/
class CTitle
	: public CSceneBase
{
public:
	// Title�Ŏg�p����X�v���C�g����ǉ����Ă���(���O��).
	enum TitleSprite {
		FullScreen,
		BestTimeText,
		Button3,
		ButtonText2,
		Button,
		ButtonText3,
		ButtonText,
		Button2,
		TitleText,
	};

public:
	CTitle(HWND hWnd);
	~CTitle();

	void Create() override;
	HRESULT LoadData() override;
	void Release() override;
	void Init() override;

	void Update() override;
	void Draw() override;

private:
	// ���Â炢�̂Ń{�^�����Ƃɏ������܂Ƃ߂�.
	void StartButtonUpdate(CKey* key, CGamePad* pad);
	void OptionButtonUpdate(CKey* key, CGamePad* pad);
	void EndButtonUpdate(CKey* key, CGamePad* pad);

	// Win���̃L�[���͋�ł�(�e�X�g�p�Ȃ̂Ō�������ΕύX���ׂ�).
	void EmptyInput();

private:
	// �摜��񃊃X�g.
	std::vector<std::string> m_SpriteDataList;//�X�v���C�g�����܂Ƃ߂�z��.

	std::vector<D3DXVECTOR3> m_SpritePosList;	//�X�v���C�g���W���܂Ƃ߂�z��.

	std::vector<CUIObject*> m_pUIs;			// UI�N���X.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2D�N���X.

	float m_ScaleDef;		//�X�P�[���̏����l.
	float m_StartScale;		//�X�^�[�g�{�^���̃X�P�[���l.
	float m_EndScale;		//�G���h�{�^���̃X�P�[���l.
	float m_OptionScale;	//�I�v�V�����{�^���̃X�P�[���l.
	float m_OpTextDefScale; //�I�v�V�����e�L�X�g�̏����X�P�[���l.

	int  m_SelectButton; //�R���g���[���[�������̓L�[�{�[�h�Ń{�^����I������Ƃ��Ɏg��.
	bool m_SelectOther;	 //�ق��̑I�����őJ�ڏ����������Ƃ��p.
};