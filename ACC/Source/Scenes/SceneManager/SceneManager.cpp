#include "SceneManager.h"

#include "Scenes/SceneBase.h"
#include "Scenes/UIEditor/UIEditor.h"
#include "Scenes/Game/Game.h"
#include "Scenes/Title/Title.h"

#include "Camera/Camera.h"
#include "Sprite/2D/UI/FadeUI/FadeUI.h"
#include "Sprite/2D/UI/EndUI/EndUI.h"
#include "DirectSound/SoundManager.h"
#include "FileManager/FileManager.h"
#include "ImGui/CImGui.h"


namespace {
	// Scene�̃p�X
	constexpr char ScenePath[] = "Data\\Scene.json";
}


SceneManager::SceneManager()
	: m_hWnd			()
	, m_SceneNo			()
	, m_NextSceneNo		()
	, m_pScene			()
	, m_FocusTime		( 0.f )
	, m_FocusTimeMax	( 35.f * Time::GetDeltaTime() )
	, m_EndDeleteFlag	( false )
{
}
SceneManager::~SceneManager()
{
}


//=============================================================================
//		�V�[���}�l�[�W���[�\�z�֐�
//=============================================================================
HRESULT SceneManager::Create(HWND hWnd)
{
	//���g�̃C���X�^���X
	auto* This = GetInstance();

	This->m_hWnd = hWnd;

	// ���߂ČĂяo���ꂽ�Ƃ��ɃC���X�^���X����
	Json m_SceneData = nullptr;	// �摜���
	m_SceneData = FileManager::JsonLoad(ScenePath);

	std::string SceneName = m_SceneData["Scene"].get<std::string>();
	if (SceneName == "Game") {
		This->m_pScene = This->CreateScene(SceneList::eGame);
	}
	else if (SceneName == "UIEditor") {
		This->m_pScene = This->CreateScene(SceneList::eUIEditor);
	}
	else {
		This->m_pScene = This->CreateScene(SceneList::eTitle);
	}
	This->m_pScene->Create();
	This->m_pScene->Init();
	This->m_pScene->LoadData();

	This->m_pFade = std::make_unique<FadeUI>();
	This->m_pFade->Create();

	return S_OK;
}


//=============================================================================
//		�X�V����
//=============================================================================
void SceneManager::Update()
{
	// �萔����
	GetInstance()->ConstantProcess();
	// �I������
	GetInstance()->EndProcess();
	// ImGui����
	GetInstance()->ImGuiUpdate();
	// �V�[���X�V����
	GetInstance()->SceneUpdate();
}


//=============================================================================
//		�`�揈��
//=============================================================================
void SceneManager::Draw()
{
	m_pScene->Draw();
	m_pFade->Draw();

	// ���݂���ꍇ�`�揈��
	if (m_pEndUI != nullptr) { m_pEndUI->Draw(); }

	if (ISDEBUG) {
		// �����ł̂�ImGui��Draw����
		CImGui::GetInstance()->Draw();
	}
}


//=============================================================================
//		�V�[���}�l�[�W���[�j��
//=============================================================================
void SceneManager::Release()
{
	m_pScene->Release();
}


//=============================================================================
//		�w�肵���V�[����ǂݍ���
//=============================================================================
void SceneManager::LoadScene(SceneList Scene)
{
	m_pFade->DoFade(30,90,30);
	m_NextSceneNo = Scene;
}


//-----------------------------------------------------------------------------
//		�w�肳�ꂽ�V�[���̐���
//-----------------------------------------------------------------------------
std::unique_ptr<SceneBase> SceneManager::CreateScene(SceneList No)
{
	m_SceneNo = No;
	RAINBOW_WINDOW = false;
	// �R���X�g���N�^��DeltaTime���g�p���Ă���ϐ��p�ɏ�����
	Time::GetInstance()->SetTimeScale(1.f);

	switch (No)
	{
	case eTitle:
		ChangeShowCursor(true);
		return std::make_unique<Title>( m_hWnd );
	case eGame:
		ChangeShowCursor(false);
		return std::make_unique<Game>( m_hWnd );
	case eUIEditor:
		ChangeShowCursor(true);
		return std::make_unique<UIEditor>( m_hWnd );
	default:
		return nullptr;
	}
	return nullptr;
}


//-----------------------------------------------------------------------------
//		�J�[�\���̕\���ؑ�
//-----------------------------------------------------------------------------
void SceneManager::ChangeShowCursor(bool flag)
{
	// �J�[�\����Ԃ̎擾
	CURSORINFO cursorInfo = {};
	cursorInfo.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&cursorInfo);

	// ShowCursor���d�����ČĂяo����Ȃ��悤�ɂ���
	if (cursorInfo.flags != CURSOR_SHOWING && flag) {
		Camera::GetInstance()->SetUseMouse(flag);
		ShowCursor(flag); 
	}
	else if (cursorInfo.flags == CURSOR_SHOWING && !flag) { 
		Camera::GetInstance()->SetUseMouse(flag);
		ShowCursor(flag);
	}
}


//-----------------------------------------------------------------------------
//		�I������
//-----------------------------------------------------------------------------
void SceneManager::EndProcess()
{
	Key* Key = DirectInput::GetInstance()->CDKeyboard();

	// ESC�������ꂽ�ꍇ
	if (Key->IsKeyAction(DIK_ESCAPE)) {
		if (m_pEndUI == nullptr) {
			ChangeShowCursor(true);

			m_pEndUI = std::make_unique<EndUI>(m_hWnd);
			m_pEndUI->Create();
			m_EndDeleteFlag = false;
		}
		else if (m_pEndUI != nullptr) {
			if (m_SceneNo == SceneList::eTitle) { ChangeShowCursor(true); }
			else { ChangeShowCursor(false); }

			m_EndDeleteFlag = true;
		}
	}

	// EndUI�폜�t���O���������ꍇ
	if (m_EndDeleteFlag) {
		if (m_SceneNo == SceneList::eTitle) { ChangeShowCursor(true); }
		else { ChangeShowCursor(false); }
		m_pEndUI.reset();
		m_EndDeleteFlag = false;
	}

	// ���݂���ꍇEndUI�X�V����
	if (m_pEndUI != nullptr) {
		m_pEndUI->Update();
		m_EndDeleteFlag = m_pEndUI->GetDeleteFlag();
	}
}


//-----------------------------------------------------------------------------
//		�萔����
//-----------------------------------------------------------------------------
void SceneManager::ConstantProcess() const
{
	BOOL isFullscreen = FALSE;
	IDXGISwapChain* swappchain = DirectX11::GetInstance()->GetSwapChain();

	BOOL isfullscreen;

#pragma warning(suppress : 6387)
	swappchain->GetFullscreenState(&isfullscreen, nullptr);
	FULLSCREEN = isfullscreen;

	// UI�Ɏg�����߂̃E�B���h�E�̃X�P�[�����v�Z
	RECT rect = {};
	GetClientRect(m_hWnd, &rect);
	NOWFWND_W = static_cast<float>(rect.right - rect.left);
	NOWFWND_H = static_cast<float>(rect.bottom - rect.top);
	FULLSCREENSCALEX = NOWFWND_W / FWND_W;
	FULLSCREENSCALEY = NOWFWND_H / FWND_H;
}


//-----------------------------------------------------------------------------
//		ImGui����
//-----------------------------------------------------------------------------
void SceneManager::ImGuiUpdate()
{
	if (!ISDEBUG) { return; }
	// �����ł̂�ImGui��Update����
	CImGui::GetInstance()->Update();

	// �V�[���I����IMGUI���쐬
	ImGui::Begin("SceneSelect");
	if (ImGui::Button("Game")) { GetInstance()->LoadScene(SceneList::eGame); }
	if (ImGui::Button("Title")) { GetInstance()->LoadScene(SceneList::eTitle); }
	ImGui::Separator();
	if (ImGui::Button("UIEditor")) { GetInstance()->LoadScene(SceneList::eUIEditor); }
	ImGui::End();

	ImGui::Begin("MousePos");
	POINT pos;
	if (GetCursorPos(&pos)) { ImGui::Text("%d,%d", pos.x, pos.y); }
	ImGui::End();
}


//-----------------------------------------------------------------------------
//		�V�[���X�V����
//-----------------------------------------------------------------------------
void SceneManager::SceneUpdate()
{
	if (m_pEndUI != nullptr) { return; }
	if (m_FocusTime > 0) { m_FocusTime -= Time::GetDeltaTime(); }

	// �t�F�[�h�̃s�[�N���ɃV�[����؂�ւ���
	if (m_pFade->GetFirstPeak()) {
		m_pScene.release();
		m_pScene = CreateScene(m_NextSceneNo);
		m_pScene->Create();
		m_pScene->Init();
		m_pScene->LoadData();
	}

	// �t�F�[�h���ȊO�AUpdate()����
	//	�t�F�[�h�̃s�[�N���Ɉ�x�����ʂ��A�t�F�[�h�����p�̔w�i���쐬����
	if (!m_pFade->GetFading() || m_pFade->GetFirstPeak()) {
		GetInstance()->m_pScene->Update();
	}

	// �t�F�[�h�I�����̂�
	if (m_pFade->GetFadeEndFrame()) {
		m_FocusTime = m_FocusTimeMax;
	}

	// �t�H�[�J�X���Ԓ��͌Œ�ŃE�B���h�E�Ƀt�H�[�J�X�����킹��
	if ( 0 < m_FocusTime) {
		SetForegroundWindow(m_hWnd);
		SetFocus(m_hWnd);
	}

	// �t�F�[�hUI�̍X�V����
	m_pFade->Update();
}