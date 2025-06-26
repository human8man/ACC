#pragma once

#include "Scenes/CSceneBase.h"
#include "Sprite/2D/UI/CUIObject.h"


//=============================================================================
//		UIEditor�N���X.
//=============================================================================
class CUIEditor
	: public CSceneBase
{
public:
	CUIEditor(HWND hWnd);
	~CUIEditor();

	void Create()		override;	// �쐬����.
	HRESULT LoadData()	override;	// �f�[�^�Ǎ�.
	void Init()			override;	// ����������.
	void Update()		override;	// �X�V����.
	void Draw()			override;	// �`�揈��.
	void Release()		override;	// �������.


private:
	// �V�F�[�_�[�Ǎ�.
	HRESULT LoadLineShader();
	// �V�F�[�_�[�X�V.
	void UpdateShader();

	// �I�������V�[����UI��ǂݍ���.
	void SelectSceneLoad(const std::string& sceneName);
	// UI�I�����ɉ��ϐ���������������.
	void SelectInit();
	// ���݃V�[����UI����ۑ�.
	HRESULT SaveScene();
	// �L�[���͏���.
	void KeyInput();

	//-------------------------------------------
	//	ImGui��p���Ă���֐�.
	//-------------------------------------------
	// �V�[���I���֐�.
	void ImGuiSelectScene();
	// UI���X�g�����֐�.
	void ImGuiSearchUI();
	// SpriteManager����UI��ǉ�.
	void AddDeleteSprite();
	// ���O�ύX���s��.
	void RenameUIObjects();

	// �I�����ꂽUI���n�C���C�g����.
	void ImGuiSetShader(CUIObject* object);
	// ���W�����֐�(�I�����ꂽUIObect).
	void ImGuiPosEdit(CUIObject* object);
	// Z���W�����Ƀ\�[�g����֐�(�I�����ꂽUIObect).
	void SortBySpritePosZ(CUIObject* object);
	// ��񒲐��֐�(�I�����ꂽUIObect).
	void ImGuiInfoEdit(CUIObject* object);
	// �p�^�[���������֐�(�I�����ꂽUIObect).
	void ImGuiPatternTest(CUIObject* object);
	// ���̑��̏�񒲐��֐�(�I�����ꂽUIObect).
	void ImGuiEtcInfoEdit(CUIObject* object);

private:
	// �V�F�[�_�[�֘A.
	ID3D11VertexShader*		m_pVertexShader;	// ���_�V�F�[�_.
	ID3D11InputLayout*		m_pInputLayout;		// ���_���C�A�E�g.
	ID3D11PixelShader*		m_pPixelShader;		// �s�N�Z���V�F�[�_.
	ID3D11GeometryShader*	m_pGeometryShader;	// �s�N�Z���V�F�[�_.
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pCBufferMatrix;		//�t���[�����̃o�b�t�@.

	std::string m_CurrentSceneName;

	float m_LineThickness;	// �n�C���C�g���̑���.
	D3DXVECTOR4 m_LineColor;// �n�C���C�g���̐F.

	// �摜��񃊃X�g.
	std::vector<std::string> m_SpriteDataList;	// �X�v���C�g�����܂Ƃ߂�z��.
	std::vector<D3DXVECTOR3> m_SpritePosList;	// �X�v���C�g���W���܂Ƃ߂�z��.

	std::vector<CUIObject*> m_pUIs;				// UI�N���X.
	std::vector<CSprite2D*> m_pSprite2Ds;		// Sprite2D�N���X.

	std::string m_ScenePath;	// ���݂̃V�[���p�X.

	float	m_DragValue;				// �����l.
	int		m_SelectedUIIndex;			// �I��UI�̃C���f�b�N�X.
	char	m_SearchBuffer[64] = "";	// �����p�o�b�t�@.

	int		m_SpriteSelectedUIIndex;	// �I�𒆒ǉ��\UI�̃C���f�b�N�X.
	char	m_SpriteSearchBuffer[64] = "";	// �����p�o�b�t�@.
	std::string m_SelectedSpriteName;	// �I�����ꂽUI��.

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