#include "CSceneManager.h"

#include "Scenes/CSceneBase.h"
#include "Scenes/Game/CGame.h"
#include "Scenes/Title/CTitle.h"

#include "Camera/CCamera.h"
#include "Sprite/2D/UI/CUIFade/CUIFade.h"
#include "DirectSound/CSoundManager.h"

#ifdef _DEBUG
	#include "Common/ImGui/CImGui.h"
#endif

//======================================================================================================================
//		�V�[���}�l�[�W���[�N���X.
//======================================================================================================================
CSceneManager::CSceneManager()
	: m_hWnd		()
	, m_pScene		()
	, m_NextSceneNo ()
{
}

CSceneManager::~CSceneManager()
{
	Release();
}


//======================================================================================================================
//		�V�[���}�l�[�W���[�\�z�֐�.
//======================================================================================================================
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


//======================================================================================================================
//		�X�V����.
//======================================================================================================================
void CSceneManager::Update()
{

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


//======================================================================================================================
//		�`�揈��.
//======================================================================================================================
void CSceneManager::Draw()
{
	m_pScene->Draw();
	m_pFade->Draw();

#ifdef _DEBUG
	// �����ł̂�ImGui��Draw����.
	CImGui::GetInstance()->Draw();
#endif
}


//======================================================================================================================
//		�V�[���}�l�[�W���[�j��.
//======================================================================================================================
void CSceneManager::Release()
{
	m_pScene->Release();
	m_pFade->Release();
}


//======================================================================================================================
//		�w�肵���V�[����ǂݍ���.
//======================================================================================================================
void CSceneManager::LoadScene(SceneList Scene)
{
	m_pFade->DoFade(120);
	m_NextSceneNo = Scene;

	CSoundManager::Stop(CSoundManager::BGM_Title);
	CSoundManager::Stop(CSoundManager::BGM_GameMain);
	CSoundManager::Stop(CSoundManager::SE_Move);
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
