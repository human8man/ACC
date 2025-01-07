#include "CGame.h"

#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "Common/DirectInput/CDirectInput.h"

#include "Effect/CEffect.h"

#include "../GameObject/Camera/CCamera.h"
#include "GameObject/Object/Ground/CGround.h"
#include "GameObject/Character/Player/CPlayer.h"
#include "GameObject/Object/Shot/CShot.h"

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
	, m_pMeshBullet		( nullptr )
	, m_pMeshGun		( nullptr )

	, m_pPlayer			( nullptr )
	, m_pGround			( nullptr )
	, m_pShot			( nullptr )
	, m_pGJK			( nullptr )
	, m_PLayerSize		{ 0.f, 2.f,0.f, }
	, m_MeshA	()
	, m_MeshB	()
{
	// ライト情報.
	m_Light.vDirection	= D3DXVECTOR3( 1.5f, 1.f, -1.f );	//ライト方向.
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
	m_pMeshBullet	= new CStaticMesh();
	m_pMeshGun		= new CStaticMesh();
	m_pPlayer		= new CPlayer();

	//地面クラスのインスタンス作成.
	m_pGround = new CGround();
	m_pGround->SetPlayer(*m_pPlayer);
	m_pShot = new CShot();
}

//============================================================================
//		ロードデータ関数.
//============================================================================
HRESULT CGame::LoadData()
{
	// スタティックメッシュの読み込み.
	m_pMeshGun		->Init( _T("Data\\Mesh\\Static\\Gun\\Gun.x" ) );
	m_pMeshFighter	->Init( _T("Data\\Mesh\\Static\\Player\\egg.x" ) );
	m_pMeshGround	->Init( _T("Data\\Mesh\\Static\\Stage\\stage.x" ) );
	m_pMeshBullet	->Init( _T("Data\\Mesh\\Static\\Bullet\\bullet.x" ) );

	m_pPlayer	->AttachMesh( *m_pMeshFighter );
	m_pGround	->AttachMesh( *m_pMeshGround );
	m_pShot		->AttachMesh( *m_pMeshBullet );

	// キャラクターの初期座標を設定.
	m_pPlayer->SetPosition( 0.f, 0.f, 6.f  );

	CCamera::GetInstance()->Init();

	return S_OK;
}


//============================================================================
//		解放関数.
//============================================================================
void CGame::Release()
{
	SAFE_DELETE(m_pShot);
	SAFE_DELETE(m_pGround);

	SAFE_DELETE(m_pPlayer);

	SAFE_DELETE(m_pMeshGun);
	SAFE_DELETE(m_pMeshBullet);
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
	CKey* Key = CDInput::GetInstance()->CDKeyboard();

	// カメラの更新.
	CCamera::GetInstance()->Update();

	// カメラ側のキー操作を無効にする.
	if (Key->IsKeyAction(DIK_F2)) { CCamera::GetInstance()->ChangeCanMove(); }
	if (Key->IsKeyAction(DIK_F3)) { CCamera::GetInstance()->ChangeUseMouse(); }
	

	// プレイヤーの更新.
	m_pPlayer->Update();

	// プレイヤーの更新後にプレイヤー座標にカメラをセット.
	if ( !CCamera::GetInstance()->GetMoveCamera())
	{	  CCamera::GetInstance()->SetPosition(m_pPlayer->GetPosition() + m_PLayerSize); }

	// 弾を飛ばす.
	//if( m_pPlayer->IsShot() ){

	//	D3DXVECTOR3 playerpos = {
	//		m_pPlayer->GetPosition().x,
	//		m_pPlayer->GetPosition().y + 1.0f,
	//		m_pPlayer->GetPosition().z };

	//	m_pShot->Reload(
	//		playerpos,
	//		m_pPlayer->GetRotation().y );
	//}

	// 弾の更新.
	//m_pShot->Update();


#if _DEBUG
	ImGui::Begin("BulletWindow");

	D3DXVECTOR3 Bullet = m_pShot->GetPosition();

	ImGui::Text("%f,%f,%f",Bullet.x,Bullet.y,Bullet.z);

	ImGui::DragFloat3("##Position", Bullet, 0.1f);

	m_pShot->SetPosition(Bullet);

	ImGui::End();
#endif


}


//============================================================================
//		描画処理.
//============================================================================
void CGame::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);

	m_pShot		->Draw( m_mView, m_mProj, m_Light );
	m_pGround	->Draw( m_mView, m_mProj, m_Light );
	m_pPlayer	->Draw( m_mView, m_mProj, m_Light );
	m_pMeshGun	->Render(m_mView, m_mProj, m_Light);

	// エフェクトの描画.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );

	CollisionJudge();
}


//-----------------------------------------------------------------------------
//		当たり判定関数.
//-----------------------------------------------------------------------------
void CGame::CollisionJudge()
{

	m_MeshB.SetVertex(
		m_pShot->GetPosition(),
		m_pShot->GetRotation(),
		m_pShot->GetScale(),
		m_pMeshBullet->GetVertices());

	m_MeshA.SetVertex(
		m_pPlayer->GetPosition(),
		m_pPlayer->GetRotation(),
		m_pPlayer->GetScale(),
		m_pMeshFighter->GetVertices());


	CollisionPoints points = m_pGJK->GJK(m_MeshA, m_MeshB);

	if (points.HasCollision)
	{
		SetWindowText(m_hWnd, L"yes");
	}
	else {
		SetWindowText(m_hWnd, L"no");
	}
}