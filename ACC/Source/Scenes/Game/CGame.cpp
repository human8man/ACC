#include "CGame.h"

#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectInput/CDirectInput.h"

#include "Effect/CEffect.h"

#include "Camera/CCamera.h"
#include "Object/Ground/CGround.h"
#include "Character/Player/CPlayer.h"

#if _DEBUG
	#include "ImGui/CImGui.h"
#endif

//============================================================================
//		ゲームクラス.
//============================================================================
CGame::CGame(HWND hWnd)
	: m_hWnd		( hWnd )
	, m_mView		()
	, m_mProj		()
	, m_Light		()

	, m_pMeshFighter	( nullptr )
	, m_pMeshGround		( nullptr )

	, m_pPlayer			( nullptr )
	, m_pGround			( nullptr )

	, m_pGJK			( nullptr )

	, m_pCamRay			( nullptr )

	, m_MeshA	()
	, m_MeshB	()

	, m_Angle	(0.f)
{
	// ライト情報.
	m_Light.vDirection	= D3DXVECTOR3( 1.5f, 1.f, -1.f );
}

CGame::~CGame()
{
	Release();
}


//============================================================================
//		構築.
//============================================================================
void CGame::Create()
{
	m_pMeshFighter	= new CStaticMesh();
	m_pMeshGround	= new CStaticMesh();
	m_pPlayer		= new CPlayer();

	// 地面クラスのインスタンス作成.
	m_pGround = new CGround();

	m_pCamRay = new CRay();
}

//============================================================================
//		ロードデータ関数.
//============================================================================
HRESULT CGame::LoadData()
{
	// スタティックメッシュの読み込み.
	m_pMeshFighter	->Init( _T("Data\\Mesh\\Static\\Player\\egg.x" ) );
	m_pMeshGround	->Init( _T("Data\\Mesh\\Static\\Stage\\stage.x" ) );

	m_pPlayer->AttachMesh( *m_pMeshFighter );
	m_pGround->AttachMesh( *m_pMeshGround );

	// キャラクターの初期座標を設定.
	m_pPlayer->SetPos( 0.f, 0.f, 6.f  );

	CCamera::GetInstance()->Init();

	
	m_pCamRay->Init(CCamera::GetInstance()->GetRay());

	return S_OK;
}


//============================================================================
//		解放関数.
//============================================================================
void CGame::Release()
{
	
	SAFE_DELETE(m_pCamRay);

	SAFE_DELETE(m_pGround);

	SAFE_DELETE(m_pPlayer);

	SAFE_DELETE(m_pMeshGround);
	SAFE_DELETE(m_pMeshFighter);

	// 外部で作成しているので、ここで破棄しない.
	m_hWnd = nullptr;
}


//============================================================================
//		初期化関数.
//============================================================================
void CGame::Init()
{
	CCamera::GetInstance()->Init();	// カメラの初期化.
}


//============================================================================
//		更新処理.
//============================================================================
void CGame::Update()
{
	CKey*	Key = CDInput::GetInstance()->CDKeyboard();

	// プレイヤーの更新.
	m_pPlayer->Update();

	// カメラの更新.
	CCamera::GetInstance()->Update();

	// カメラ側のキー操作を無効にする.
	if (Key->IsKeyAction(DIK_F2)) { CCamera::GetInstance()->ChangeCanMove(); }
	if (Key->IsKeyAction(DIK_F3)) { CCamera::GetInstance()->ChangeUseMouse(); }
}


//============================================================================
//		描画処理.
//============================================================================
void CGame::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);

	m_pGround->Draw( m_mView, m_mProj, m_Light );
	m_pPlayer->Draw( m_mView, m_mProj, m_Light );

	m_pCamRay->Render(m_mView, m_mProj, CCamera::GetInstance()->GetRay());

	// エフェクトの描画.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );
	CollisionJudge();
}


//-----------------------------------------------------------------------------
//		当たり判定関数.
//-----------------------------------------------------------------------------
void CGame::CollisionJudge()
{
	//// 弾とプレイヤーの判定.
	//for (size_t i = 0; i < m_pBullets.size(); ++i) {

	//	m_MeshB.SetVertex(
	//		m_pBullets[i]->GetPos(),
	//		m_pBullets[i]->GetRot(),
	//		m_pBullets[i]->GetScale(),
	//		m_pMeshBullet->GetVertices());

	//	m_MeshA.SetVertex(
	//		m_pPlayer->GetPos(),
	//		m_pPlayer->GetRot(),
	//		m_pPlayer->GetScale(),
	//		m_pMeshFighter->GetVertices());

	//	CollisionPoints points = m_pGJK->GJK(m_MeshA, m_MeshB);

	//	// あたった場合削除.
	//	if (points.HasCollision)
	//	{
	//		SAFE_DELETE(m_pBullets[i]);
	//		m_pBullets.erase(m_pBullets.begin() + i);
	//		--i;
	//	}
	//}

	//auto [hit, hitpos, length] = m_pGround->IsHitForRay(CCamera::GetInstance()->GetRay());
	
//#if _DEBUG
//	// 銃ウィンドウ.
//	ImGui::Begin("CameraWindow");
//
//	D3DXVECTOR3 camrot = CCamera::GetInstance()->GetRot();
//	ImGui::Text("%f,%f,%f", camrot.x, camrot.y, camrot.z);
//	ImGui::DragFloat3("##Position", camrot, 0.1f);
//
//	ImGui::End();
//
//
//	// 銃ウィンドウ.
//	ImGui::Begin("BulletWindow");
//
//	D3DXVECTOR3 shootpos = m_pGun->GetPos();
//	ImGui::Text("%f,%f,%f", shootpos.x, shootpos.y, shootpos.z);
//	ImGui::DragFloat3("##Position", shootpos, 0.1f);
//	m_pGun->SetPos(shootpos);
//
//	ImGui::End();
//
//
//	// 弾のウィンドウ.
//	ImGui::Begin("BulletWindow");
//
//	for (size_t i = 0; i < m_pBullets.size(); ++i) {
//		D3DXVECTOR3 rot = m_pBullets[i]->GetRot();
//		D3DXVECTOR3 pos = m_pBullets[i]->GetPos();
//		ImGui::Text("------------------------------------");
//		ImGui::DragFloat3("rot", rot, 0.01f);
//		ImGui::DragFloat3("pos", pos, 0.01f);
//	}
//	ImGui::End();
//
//
//	// レイの当たり判定のウィンドウ.
//	ImGui::Begin("ColWindow");
//
//	if(hit) { ImGui::Text("true");	}
//	else	{ ImGui::Text("false");	}
//	ImGui::Text("%f,%f,%f",hitpos.x,hitpos.y,hitpos.z );
//	ImGui::Text("%f",length );
//
//	ImGui::End();
//#endif
}