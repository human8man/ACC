#include "CTitle.h"

#include "Camera/CCamera.h"
#include "Mesh/Static/CStaticMesh.h"
#include "DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "Sprite/2D/UI/TitleUI/TitleUI.h"


//=================================================================================================
//		タイトルクラス
//=================================================================================================
CTitle::CTitle(HWND hWnd)
	: m_Light			()
	, m_mView			()
	, m_mProj			()
	, m_pTitleUI		( nullptr )
	, m_pEgg			()
	, m_Start			( false )
{
	m_hWnd = hWnd;
	m_Light.vDirection = D3DXVECTOR3(1.f, 5.f, 0.f);	//ライト方向
}
CTitle::~CTitle()
{
	Release();
}


//=================================================================================================
//		作成処理
//=================================================================================================
void CTitle::Create()
{
	CDirectX11::GetInstance()->SetChangeDebugBuffer(false);
	m_pTitleUI = std::make_unique<TitleUI>();
	m_pTitleUI->Create();
	m_pEgg = new CStaticMesh();
}


//=================================================================================================
//		読込処理
//=================================================================================================
HRESULT CTitle::LoadData()
{
	m_pEgg->Init(_T("Data\\Mesh\\Static\\Player\\egg.x"));
	m_pEgg->SetPos(D3DXVECTOR3(10.f, -1.f, 4.f));

	return S_OK;
}


//=================================================================================================
//		初期化処理
//=================================================================================================
void CTitle::Init()
{
	m_Start = false;
	CCamera::GetInstance()->Init();	// カメラの初期化
}


//=================================================================================================
//		更新処理
//=================================================================================================
void CTitle::Update()
{
	// BGM再生
	CSoundManager::GetInstance()->PlayLoop(CSoundManager::enList::BGM_Title);
	m_pTitleUI->Update();
	m_pEgg->SetRotY(m_pEgg->GetRot().y + 0.01f);

	if (m_pTitleUI->GetStart()) {
		// ゲームを開始する
		CSceneManager::GetInstance()->LoadScene(SceneList::Game);
		CSoundManager::GetInstance()->AllStop();
	}
}


//=================================================================================================
//		描画処理
//=================================================================================================
void CTitle::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);
	m_pTitleUI->Draw();
	m_pEgg->Render(m_mView, m_mProj, m_Light);
}


//=================================================================================================
//		解放処理
//=================================================================================================
void CTitle::Release()
{
	m_pTitleUI.reset();
	SAFE_DELETE(m_pEgg);
}