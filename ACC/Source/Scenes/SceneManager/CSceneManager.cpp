#include "CSceneManager.h"

#include "Scenes/CSceneBase.h"
#include "Scenes/Game/CGame.h"

#include "Camera/CCamera.h"
#include "Sprite/2D/UI/CUIFade/CUIFade.h"

#include "DirectSound/CSoundManager.h"


#ifdef _DEBUG
	#include "Common/ImGui/CImGui.h"
#endif // _DEBUG.



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

//�V�[���}�l�[�W���[�\�z�֐�
HRESULT CSceneManager::Create(HWND hWnd)
{
	//���g�̃C���X�^���X
	auto* This = GetInstance();

	This->m_hWnd = hWnd;

	//���߂ČĂяo���ꂽ�Ƃ��ɃC���X�^���X����
	This->m_pScene = This->CreateScene(SceneList::Game);
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

//�X�V����
void CSceneManager::Update()
{

#ifdef _DEBUG
	// Debug�ł̎��s���̂ݍX�V.
	CImGui::GetInstance()->Update();
#endif
	 
	if (m_pFade->GetFadPeak()) {
 		m_pScene.release();
		m_pScene = CreateScene(m_NextSceneNo);
		m_pScene->Create();
		m_pScene->LoadData();
	}

#ifdef _DEBUG

	ImGui::Begin("SceneSelect");
	if (ImGui::Button("Game"))
	{
		GetInstance()->LoadScene(SceneList::Game);
	}
	ImGui::End();
#endif

	if (!m_pFade->GetFading()
	|| m_pFade->GetFadPeak()) 
	{
		GetInstance()->m_pScene->Update();
	}
	
	m_pFade->Update();
}

//�`�揈��
void CSceneManager::Draw()
{
	m_pScene->Draw();

#ifdef _DEBUG

	ImGui::Begin("Camera");
	float CameraViewAngle = CCamera::GetViewAngle();
	ImGui::DragFloat("View Angle", &CameraViewAngle, 1.0f, 1.0f,120.0f);
	float Sens = CCamera::GetInstance()->GetSens();
	ImGui::DragFloat("Sens", &Sens, 1.0f, 0.01f, 1.0f);
	CCamera::SetViewAngle(CameraViewAngle);
	ImGui::End();

	// Debug�ł̎��s���̂ݕ`��.
	CImGui::GetInstance()->Draw();
#endif
}

//�V�[���}�l�[�W���[�j��
void CSceneManager::Release()
{
	m_pScene->Release();

	m_pFade->Release();
}

//�w�肵���V�[����ǂݍ���
void CSceneManager::LoadScene(SceneList Scene)
{
	m_pFade->DoFade(120);
	m_NextSceneNo = Scene;

	CSoundManager::Stop(CSoundManager::BGM_Title);
	CSoundManager::Stop(CSoundManager::BGM_GameMain);
	CSoundManager::Stop(CSoundManager::SE_Move);
}

//�w�肳�ꂽ�V�[���̐���
std::unique_ptr<CSceneBase> CSceneManager::CreateScene(SceneList No)
{
	switch (No)
	{
	case Game: 
		ShowCursor(false);

		return std::make_unique<CGame>( m_hWnd );


	default:return nullptr;
	}
    return nullptr;
}
