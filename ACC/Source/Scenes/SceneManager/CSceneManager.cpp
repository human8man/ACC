#include "CSceneManager.h"

#include "Scenes/CSceneBase.h"
#include "Scenes/Game/CGame.h"
#include "Scenes/Title/CTitle.h"

#include "Camera/CCamera.h"
#include "Sprite/2D/UI/CUIFade/CUIFade.h"
#include "Sprite/2D/UI/CEndUI/CEndUI.h"
#include "DirectSound/CSoundManager.h"

#ifdef _DEBUG
	#include "Common/ImGui/CImGui.h"
#endif

//=================================================================================================
//		�V�[���}�l�[�W���[�N���X.
//=================================================================================================
CSceneManager::CSceneManager()
	: m_hWnd			()
	, m_pScene			()
	, m_NextSceneNo		()
	, m_EndDeleteFlag	( false )
{
}

CSceneManager::~CSceneManager()
{
	Release();
}


//=================================================================================================
//		�V�[���}�l�[�W���[�\�z�֐�.
//=================================================================================================
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


//=================================================================================================
//		�X�V����.
//=================================================================================================
void CSceneManager::Update()
{
	CKey* Key = CDInput::GetInstance()->CDKeyboard();

	// ESC�������ꂽ�ꍇ.
	if (Key->IsKeyAction(DIK_ESCAPE)) {
		// ENDUI�����݂��Ȃ��ꍇ�o��.
		if (m_pEndUI == nullptr) {
			m_pEndUI = std::make_unique<CEndUI>();
			m_pEndUI->Create(m_hWnd);
			m_EndDeleteFlag = false;
		}
		// ENDUI�����݂���ꍇ�폜.
		else if (m_pEndUI != nullptr) {
			m_EndDeleteFlag = true;
		}
	}

	// EndUI�폜�t���O���������ꍇ.
	if (m_EndDeleteFlag) {
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

	ImGui::Begin("SceneSelect");
	if (ImGui::Button("Game")) {
		GetInstance()->LoadScene(SceneList::Game);
	}
	if (ImGui::Button("Title")) {
		GetInstance()->LoadScene(SceneList::Title);
	}
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

		// �t�F�[�h���łȂ��ԁAUpdate()����.
		//	�t�F�[�h�̃s�[�N���Ɉ�x�����ʂ��A�t�F�[�h�����p�̔w�i���쐬����.
		if (!m_pFade->GetFading() || m_pFade->GetFadePeak())
		{
			GetInstance()->m_pScene->Update();
		}

		m_pFade->Update();
	}

#if _DEBUG

	ImGui::Begin("MousePos");
	POINT pos;
	if (GetCursorPos(&pos)) {
		ImGui::Text("%d,%d", pos.x, pos.y);
	}
	ImGui::End();

#endif
}


//=================================================================================================
//		�`�揈��.
//=================================================================================================
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


//=================================================================================================
//		�V�[���}�l�[�W���[�j��.
//=================================================================================================
void CSceneManager::Release()
{
	m_pScene->Release();
	m_pFade->Release();
}


//=================================================================================================
//		�w�肵���V�[����ǂݍ���.
//=================================================================================================
void CSceneManager::LoadScene(SceneList Scene)
{
	m_pFade->DoFade(120);
	m_NextSceneNo = Scene;
}


//----------------------------------------------------------------------------------------------------------------------
//		�w�肳�ꂽ�V�[���̐���.
//----------------------------------------------------------------------------------------------------------------------
std::unique_ptr<CSceneBase> CSceneManager::CreateScene(SceneList No)
{
	m_SceneNo = No;
	switch (No)
	{
	case Title:
		return std::make_unique<CTitle>( m_hWnd );
	case Game:
		return std::make_unique<CGame>( m_hWnd );
	default:return nullptr;
	}
	return nullptr;
}
