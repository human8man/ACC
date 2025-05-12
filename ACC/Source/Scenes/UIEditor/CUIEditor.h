#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"


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
	// UI�I�����ɉ��ϐ���������������.
	void SelectInit();
	// ���݃V�[����UI����ۑ�.
	HRESULT SaveScene();

	//-------------------------------------------
	//	ImGui��p���Ă���֐�.
	//-------------------------------------------
	// �V�[���I���֐�.
	void ImGuiSelectScene();
	// UI���X�g�����֐�.
	void ImGuiSearchUI();
	// ���W�����֐�(�I�����ꂽUIObect).
	void ImGuiPosEdit(CUIObject* object);
	// Z���W�����Ƀ\�[�g����֐�.
	void SortBySpritePosZ(CUIObject* object);
	// ��񒲐��֐�(�I�����ꂽUIObect).
	void ImGuiInfoEdit(CUIObject* object);
	// �p�^�[���������֐�(�I�����ꂽUIObect).
	void ImGuiPatternTest(CUIObject* object);
	// ���̑��̏�񒲐��֐�(�I�����ꂽUIObect).
	void ImGuiEtcInfoEdit(CUIObject* object);

private:
	// �摜��񃊃X�g.
	std::vector<std::string> m_SpriteDataList;	//�X�v���C�g�����܂Ƃ߂�z��.
	std::vector<D3DXVECTOR3> m_SpritePosList;	//�X�v���C�g���W���܂Ƃ߂�z��.

	std::vector<CUIObject*> m_pUIs;				// UI�N���X.
	std::vector<CSprite2D*> m_pSprite2Ds;		// Sprite2D�N���X.

	std::string m_ScenePath;	// ���݂̃V�[���p�X.

	bool	m_MovedSomething;	// �����ύX���������ۂɕۑ��m�F�t���O�𗧂Ă�.
	int		m_SelectedUIIndex;
	char	m_SearchBuffer[64] = ""; // �����p�o�b�t�@.
	
	// �}�E�X����p�̕ϐ�.
	D3DXVECTOR2 m_OffsetPos;	// �}�E�X���W�Ɖ摜���W�̃Y���␳�l.
	bool m_DoDrag;				// �h���b�O��.
	bool m_MovedSpritePos;		// �摜���W���ύX���ꂽ�ۂ̃t���O.

	// �p�^�[���m�F�p�̕ϐ�.
	POINTS	m_PatternNo;		// ���̃p�^�[��.
	POINTS	m_PatternMax;		// ���̍ő�p�^�[��.
	bool	m_PatternAuto;		// �p�^�[���������Ői�߂�.
	float	m_AnimationSpeed;	// ���葬�x(�t���[��).
	float	m_AnimationSpeedMax;// �ő呗�葬�x(�t���[��).
};