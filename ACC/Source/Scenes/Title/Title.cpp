#include "Title.h"

#include "Camera/Camera.h"
#include "Mesh/Static/StaticMesh.h"
#include "DirectInput/DirectInput.h"
#include "DirectSound/SoundManager.h"
#include "Scenes/SceneManager/SceneManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "Sprite/2D/UI/TitleUI/TitleUI.h"


Title::Title(HWND hWnd)
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
Title::~Title()
{
	Release();
}


//=================================================================================================
//		作成処理
//=================================================================================================
void Title::Create()
{
	DirectX11::GetInstance()->SetChangeDebugBuffer(false);
	m_pTitleUI = std::make_unique<TitleUI>();
	m_pTitleUI->Create();
	m_pEgg = new StaticMesh();
}


//=================================================================================================
//		読込処理
//=================================================================================================
HRESULT Title::LoadData()
{
	m_pEgg->Init(_T("Data\\Mesh\\Static\\Player\\egg.x"));
	m_pEgg->SetPos(D3DXVECTOR3(10.f, -1.f, 4.f));

	return S_OK;
}


//=================================================================================================
//		初期化処理
//=================================================================================================
void Title::Init()
{
	m_Start = false;
	Camera::GetInstance()->Init();	// カメラの初期化
}


//=================================================================================================
//		更新処理
//=================================================================================================
void Title::Update()
{
	// BGM再生
	SoundManager::GetInstance()->PlayLoop(SoundManager::enList::BGM_Title);
	m_pTitleUI->Update();

	if (m_pTitleUI->GetStart()) {
		// ゲームを開始する
		SceneManager::GetInstance()->LoadScene(SceneList::eGame);
		SoundManager::GetInstance()->AllStop();
	}
}


//=================================================================================================
//		描画処理
//=================================================================================================
void Title::Draw()
{
	Camera::GetInstance()->CameraView(m_mView);
	SceneBase::Projection(m_mProj);
	m_pTitleUI->Draw();
	m_pEgg->Draw(m_mView, m_mProj, m_Light);
}


//=================================================================================================
//		解放処理
//=================================================================================================
void Title::Release()
{
	m_pTitleUI.reset();
	SAFE_DELETE(m_pEgg);
}