#include "CSceneManager.h"

#include "Scenes/CSceneBase.h"
#include "Scenes/UIEditor/CUIEditor.h"
#include "Scenes/Game/CGame.h"
#include "Scenes/Title/CTitle.h"

#include "Camera/CCamera.h"
#include "Sprite/2D/UI/CUIFade/CUIFade.h"
#include "Sprite/2D/UI/CEndUI/CEndUI.h"
#include "DirectSound/CSoundManager.h"


#ifdef _DEBUG
	#include "ImGui/CImGui.h"
#endif


//=============================================================================
//		�V�[���}�l�[�W���[�N���X.
//=============================================================================
CSceneManager::CSceneManager()
	: m_hWnd			()
	, m_SceneNo			()
	, m_NextSceneNo		()
	, m_pScene			()
	, m_EndDeleteFlag	( false )
{
}

CSceneManager::~CSceneManager()
{
	Release();
}


//=============================================================================
//		�V�[���}�l�[�W���[�\�z�֐�.
//=============================================================================
HRESULT CSceneManager::Create(HWND hWnd)
{
	//���g�̃C���X�^���X
	auto* This = GetInstance();

	This->m_hWnd = hWnd;

	//���߂ČĂяo���ꂽ�Ƃ��ɃC���X�^���X����
	This->m_pScene = This->CreateScene(SceneList::Title);
	This->m_pScene->Create();
	This->m_pScene->Init();
	This->m_pScene->LoadData();

	This->m_pFade = std::make_unique<CUIFade>();
	This->m_pFade->Create();


	// �T�E���h�f�[�^�̓ǂݍ���.
	if (CSoundManager::GetInstance()->Load(hWnd) == false) {
		return E_FAIL;
	}

	return S_OK;
}


//=============================================================================
//		�X�V����.
//=============================================================================
void CSceneManager::Update()
{
	CKey* Key = CInput::GetInstance()->CDKeyboard();
	BOOL isFullscreen = FALSE;
	IDXGISwapChain* swappchain = CDirectX11::GetInstance()->GetSwapChain();

	BOOL isfullscreen;

#pragma warning(suppress : 6387)
	swappchain->GetFullscreenState(&isfullscreen, nullptr);
	FULLSCREEN = isfullscreen;

	// UI�Ɏg�����߂̃E�B���h�E�̃X�P�[�����v�Z.
	RECT rect = {};
	GetClientRect(m_hWnd, &rect);
	NOWFWND_W = static_cast<float>(rect.right - rect.left);
	NOWFWND_H = static_cast<float>(rect.bottom - rect.top);
	FULLSCREENSCALEX = NOWFWND_W / FWND_W;
	FULLSCREENSCALEY = NOWFWND_H / FWND_H;

	// ESC�������ꂽ�ꍇ.
	if (Key->IsKeyAction(DIK_ESCAPE)) {
		if (m_pEndUI == nullptr) {
			ChangeShowCursor(true);

			m_pEndUI = std::make_unique<CEndUI>();
			m_pEndUI->Create(m_hWnd);
			m_EndDeleteFlag = false;
		}
		else if (m_pEndUI != nullptr) {
			if (m_SceneNo == SceneList::Title) { ChangeShowCursor(true); }
			else{ ChangeShowCursor(false); }

			m_EndDeleteFlag = true;
		}
	}

	// EndUI�폜�t���O���������ꍇ.
	if (m_EndDeleteFlag) {
		if (m_SceneNo == SceneList::Title) { ChangeShowCursor(true); }
		else { ChangeShowCursor(false); }
		m_pEndUI.reset();
		m_EndDeleteFlag = false;
	}

	// ���݂���ꍇEndUI�X�V����.
	if (m_pEndUI != nullptr) { 
		m_pEndUI->Update(); 
		m_EndDeleteFlag = m_pEndUI->GetDeleteFlag();
	}

#ifdef _DEBUG
	// �����ł̂�ImGui��Update����.
	CImGui::GetInstance()->Update();

	// �V�[���I����IMGUI���쐬.
	ImGui::Begin("SceneSelect");
	if (ImGui::Button("Game")) { GetInstance()->LoadScene(SceneList::Game); }
	if (ImGui::Button("Title")) { GetInstance()->LoadScene(SceneList::Title); }

	ImGui::Text("\n");
	ImGui::Text(IMGUI_JP("-----�f�o�b�O�p�V�[��------------------------------------"));
	if (ImGui::Button("UIEditor")) { GetInstance()->LoadScene(SceneList::UIEditor); }
	ImGui::End();

	ImGui::Begin("MousePos");
	POINT pos;
	if (GetCursorPos(&pos)) { ImGui::Text("%d,%d", pos.x, pos.y); }
	ImGui::End();
#endif

	if (m_pEndUI == nullptr) {
		// �t�F�[�h�̃s�[�N���ɃV�[����؂�ւ���.
		if (m_pFade->GetFadePeak()) {
			m_pScene.release();
			m_pScene = CreateScene(m_NextSceneNo);
			m_pScene->Create();
			m_pScene->Init();
			m_pScene->LoadData();
		}

		// �t�F�[�h���ȊO�AUpdate()����.
		//	�t�F�[�h�̃s�[�N���Ɉ�x�����ʂ��A�t�F�[�h�����p�̔w�i���쐬����.
		if (!m_pFade->GetFading() || m_pFade->GetFadePeak()) {
			GetInstance()->m_pScene->Update();
		}

		// �t�F�[�hUI�̍X�V����.
		m_pFade->Update();
	}
}


//=============================================================================
//		�`�揈��.
//=============================================================================
void CSceneManager::Draw()
{
	m_pScene->Draw();
	m_pFade->Draw();

	// ���݂���ꍇ�`�揈��.
	if (m_pEndUI != nullptr) { m_pEndUI->Draw(); }

#ifdef _DEBUG
	// �����ł̂�ImGui��Draw����.
	CImGui::GetInstance()->Draw();
#endif
}


//=============================================================================
//		�V�[���}�l�[�W���[�j��.
//=============================================================================
void CSceneManager::Release()
{
	//m_pScene->Release();
	m_pFade->Release();
}


//=============================================================================
//		�w�肵���V�[����ǂݍ���.
//=============================================================================
void CSceneManager::LoadScene(SceneList Scene)
{
	m_pFade->DoFade(120);
	m_NextSceneNo = Scene;
}


//-----------------------------------------------------------------------------
//		�w�肳�ꂽ�V�[���̐���.
//-----------------------------------------------------------------------------
std::unique_ptr<CSceneBase> CSceneManager::CreateScene(SceneList No)
{
	m_SceneNo = No;

	switch (No)
	{
	case Title:
		ChangeShowCursor(true);
		return std::make_unique<CTitle>( m_hWnd );
	case Game:
		ChangeShowCursor(false);
		return std::make_unique<CGame>( m_hWnd );
	case UIEditor:
		ChangeShowCursor(true);
		return std::make_unique<CUIEditor>( m_hWnd );
	default:
		return nullptr;
	}
	return nullptr;
}


//-----------------------------------------------------------------------------
//		�J�[�\���̕\���ؑ�.
//-----------------------------------------------------------------------------
void CSceneManager::ChangeShowCursor(bool flag)
{
	// �J�[�\����Ԃ̎擾.
	CURSORINFO cursorInfo = {};
	cursorInfo.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&cursorInfo);

	// ShowCursor���d�����ČĂяo����Ȃ��悤�ɂ���.
	if (cursorInfo.flags != CURSOR_SHOWING && flag) {
		CCamera::GetInstance()->SetUseMouse(flag);
		ShowCursor(flag); 
	}
	else if (cursorInfo.flags == CURSOR_SHOWING && !flag) { 
		CCamera::GetInstance()->SetUseMouse(flag);
		ShowCursor(flag);
	}
}