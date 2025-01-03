#include "CGame.h"

#include "DirectSound/CSoundManager.h"

#include "Effect/CEffect.h"	//Effekseerを使うためのクラス.
#include "../GameObject/Camera/CCamera.h"

//============================================================================
//		ゲームクラス.
//============================================================================
CGame::CGame(HWND hWnd)
	: m_hWnd		( hWnd )
	, m_mView		()
	, m_mProj		()
	, m_Light		()



	, m_pStaticMeshFighter	( nullptr )
	, m_pStaticMeshGround	( nullptr )
	, m_pStaticMeshBullet	( nullptr )


	, m_pPlayer				( nullptr )

	, m_pGround				( nullptr )

	, m_pShot				( nullptr )
{
	// ライト情報.
	m_Light.vDirection	= D3DXVECTOR3( 1.5f, 1.f, -1.f );	//ライト方向.
}

CGame::~CGame()
{
	SAFE_DELETE( m_pShot );		// 弾の破棄.
	SAFE_DELETE( m_pGround );	// 地面の破棄.
	
	SAFE_DELETE( m_pPlayer );		// プレイヤーの破棄.

	SAFE_DELETE( m_pStaticMeshBullet );
	SAFE_DELETE( m_pStaticMeshGround );
	SAFE_DELETE( m_pStaticMeshFighter );

	//外部で作成しているので、ここで破棄しない.
	m_hWnd = nullptr;
}

// 構築.
void CGame::Create()
{
	m_pStaticMeshFighter	= new CStaticMesh();
	m_pStaticMeshGround		= new CStaticMesh();
	m_pStaticMeshBullet		= new CStaticMesh();
	m_pPlayer				= new CPlayer();

	//地面クラスのインスタンス作成.
	m_pGround = new CGround();
	m_pGround->SetPlayer(*m_pPlayer);
	m_pShot = new CShot();
}

// ロードデータ関数.
HRESULT CGame::LoadData()
{

	// 地面スプライトの構造体.
	CSprite3D::SPRITE_STATE SSGround{ 1.f, 1.f, 256.f, 256.f, 256.f, 256.f };

	// プレイヤースプライトの構造体.
	CSprite3D::SPRITE_STATE SSPlayer = { 1.f, 1.f, 64.f, 64.f, 64.f, 64.f };

	// 爆発プライトの構造体.
	CSprite3D::SPRITE_STATE SSExplosion = { 1.f, 1.f, 256.f, 256.f, 32.f, 32.f };

	// スタティックメッシュの読み込み.
	m_pStaticMeshFighter->Init( _T("Data\\Mesh\\Static\\Player\\egg.x" ) );
	m_pStaticMeshGround	->Init( _T("Data\\Mesh\\Static\\Stage\\stage.x" ) );
	m_pStaticMeshBullet	->Init( _T("Data\\Mesh\\Static\\Bullet\\bullet.x" ) );

	m_pPlayer		->AttachMesh( *m_pStaticMeshFighter );
	m_pGround		->AttachMesh( *m_pStaticMeshGround );
	m_pShot			->AttachMesh( *m_pStaticMeshBullet );

	m_pShot->CreateBSphereForMesh( *m_pStaticMeshBullet );

	// キャラクターの初期座標を設定.
	m_pPlayer->SetPosition( 0.f, 1.f, 6.f  );

	CCamera::GetInstance()->Init();
	CCamera::SetPlayerPos(m_pPlayer->GetPosition());

	return S_OK;
}

// 解放関数.
void CGame::Release()
{
}

void CGame::Init()
{
	CCamera::GetInstance()->Init();	// カメラの初期化.
}


// 更新処理.
void CGame::Update()
{
	CCamera::GetInstance()->Update();

	m_pPlayer->Update();

	m_pGround->Update();
	
	//弾を飛ばしたい！.
	if( m_pPlayer->IsShot() == true ){
		m_pShot->Reload(
			m_pPlayer->GetPosition(),
			m_pPlayer->GetRotation().y );
	}

	m_pShot->Update();
}

//描画処理.
void CGame::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);

	m_pGround->Draw( m_mView, m_mProj, m_Light );

	m_pPlayer->Draw( m_mView, m_mProj, m_Light );

	m_pShot->Draw( m_mView, m_mProj, m_Light );


	//当たり判定の中心座標を更新する.
	m_pPlayer->UpdateBSpherePos();
	m_pShot->UpdateBSpherePos();

	//弾とエネミーの当たり判定.
	if( m_pShot->GetBSphere()->IsHit( *m_pGround->GetBSphere() ) )
	{
		//弾.
		m_pShot->SetDisplay( false );				//非表示.
		m_pShot->SetPosition( 0.f, -10.f, 0.f );	//地面に埋める.
	}

	//Effectクラス.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );
}