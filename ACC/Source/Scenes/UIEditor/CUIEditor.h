#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"
#include "DirectInput/CDirectInput.h"


//=============================================================================
//		WinUI�N���X.
//=============================================================================
class CUIEditor
	: public CSceneBase
{
private:
	// �ύX�\��UI�̃��X�g.
	enum UISceneList {
		Title,
		Game,
		Lose,
		Win,
	};
public:
	CUIEditor(HWND hWnd);
	~CUIEditor();

	void Create()	override;	// �쐬����.
	HRESULT LoadData()override;	// �f�[�^�Ǎ�.
	void Init()		override;	// ����������.
	void Update()	override;	// �X�V����.
	void Draw()		override;	// �`�揈��.
	void Release()	override;	// �������.


private:
	// �I�������V�[����UI��ǂݍ���.
	void SelectSceneLoad(UISceneList scene);
	// ���݃V�[����UI����ۑ�.
	HRESULT SaveScene();

private:
	// �摜��񃊃X�g.
	std::vector<std::string> m_SpriteDataList;	//�X�v���C�g�����܂Ƃ߂�z��.
	std::vector<D3DXVECTOR3> m_SpritePosList;	//�X�v���C�g���W���܂Ƃ߂�z��.

	std::vector<CUIObject*> m_pUIs;				// UI�N���X.
	std::vector<CSprite2D*> m_pSprite2Ds;		// Sprite2D�N���X.

	std::string m_ScenePath;	// ���݂̃V�[���p�X.
	D3DXVECTOR2 m_OffsetPos;	// �}�E�X���W�Ɖ摜���W�̃Y���␳�l.

	bool m_MovedSomething;		// �����ύX���������ۂɕۑ��m�F�t���O�𗧂Ă�.
};