#include "CGame.h"

#include "DirectSound/CSoundManager.h"

#include "Effect/CEffect.h"	//Effekseerを使うためのクラス.
#include "../GameObject/Camera/CCamera.h"

//============================================================================
//		ゲームクラス.
//============================================================================
CGame::CGame(HWND hWnd)
	: m_pDx9		( CDirectX9::GetInstance() )
	, m_pDx11		( CDirectX11::GetInstance() )
	, m_pDbgText	( nullptr )
	, m_pRayY		( nullptr )
	, m_pCrossRay	()
	, m_hWnd		( hWnd )
	, m_mView		()
	, m_mProj		()
	, m_Light		()


	, m_pSpriteGround		( nullptr )
	, m_pSpritePlayer		( nullptr )
	, m_pSpriteExplosion	( nullptr )

	, m_pStaticMeshFighter	( nullptr )
	, m_pStaticMeshGround	( nullptr )
	, m_pStaticMeshRoboA	( nullptr )
	, m_pStaticMeshRoboB	( nullptr )
	, m_pStaticMeshBullet	( nullptr )
	, m_pStaticMeshBSphere	( nullptr )

	, m_pSkinMeshZako		( nullptr )
	, m_ZakoAnimNo			()
	, m_ZakoAnimTime		()
	, m_ZakoBonePos			()

	, m_pExplosion			( nullptr )

	, m_pStcMeshObj			( nullptr )
	, m_pPlayer				( nullptr )
	, m_pEnemy				( nullptr )
	, m_pEnemies			()
	, m_ppEnemies			( nullptr )
	, m_EnemyMax			( 0 )

	, m_pGround				( nullptr )

	, m_pShot				( nullptr )

	, m_pZako				( nullptr )
	, m_Zako				()
{
	// ライト情報.
	m_Light.vDirection	= D3DXVECTOR3( 1.5f, 1.f, -1.f );	//ライト方向.
}

CGame::~CGame()
{
	// ザコ複数.
	for (auto it = m_Zako.rbegin(); it != m_Zako.rend(); ++it){
		SAFE_DELETE( *it );
	}
	
	SAFE_DELETE( m_pZako );		// ザコの破棄.
	SAFE_DELETE( m_pShot );		// 弾の破棄.
	SAFE_DELETE( m_pGround );	// 地面の破棄.
	
	// エネミーの破棄.
	for( int No = ENEMY_MAX - 1; No >= 0; No-- ){
		SAFE_DELETE( m_pEnemies[No] );
	}

	SAFE_DELETE( m_pEnemy );
	SAFE_DELETE( m_pPlayer );		// プレイヤーの破棄.
	SAFE_DELETE( m_pStcMeshObj );	// スタティックメッシュオブジェクトの破棄.
	SAFE_DELETE( m_pExplosion );	// 爆発の破棄.
	SAFE_DELETE( m_pSkinMeshZako );	// スキンメッシュの破棄.

	// スタティックメッシュの破棄.
	SAFE_DELETE( m_pStaticMeshBSphere );
	SAFE_DELETE( m_pStaticMeshBullet );
	SAFE_DELETE( m_pStaticMeshRoboB );
	SAFE_DELETE( m_pStaticMeshRoboA );
	SAFE_DELETE( m_pStaticMeshGround );
	SAFE_DELETE( m_pStaticMeshFighter );

	SAFE_DELETE( m_pSpriteExplosion );	// 爆発スプライトの解放.
	SAFE_DELETE( m_pSpritePlayer );		// プレイヤースプライトの解放.
	SAFE_DELETE( m_pSpriteGround );		// 地面スプライトの解放.

	// レイ表示クラスの破棄.
	for (int dir = CROSSRAY::max - 1; dir >= 0; --dir) {
		SAFE_DELETE(m_pCrossRay[dir]);
	}

	SAFE_DELETE( m_pRayY );
	SAFE_DELETE( m_pDbgText );	// デバッグテキストの破棄.

	//外部で作成しているので、ここで破棄しない.
	m_hWnd = nullptr;
}

// 構築.
void CGame::Create()
{
	m_pDbgText = new CDebugText();
	// レイ表示クラスのインスタンス作成.
	m_pRayY = new CRay();
	for (int dir = 0; dir < CROSSRAY::max; ++dir){
		m_pCrossRay[dir] = new CRay();
	}
	m_pSpriteGround			= new CSprite3D();
	m_pSpritePlayer			= new CSprite3D();
	m_pSpriteExplosion		= new CSprite3D();
	m_pStaticMeshFighter	= new CStaticMesh();
	m_pStaticMeshGround		= new CStaticMesh();
	m_pStaticMeshRoboA		= new CStaticMesh();
	m_pStaticMeshRoboB		= new CStaticMesh();
	m_pStaticMeshBullet		= new CStaticMesh();
	m_pStaticMeshBSphere	= new CStaticMesh();
	m_pSkinMeshZako			= new CSkinMesh();
	m_pExplosion			= new CExplosion();
	m_pStcMeshObj			= new CStaticMeshObject();
	m_pPlayer				= new CPlayer();
	m_pEnemy				= new CEnemy();
#if 1
	//エネミーを動的に確保.
	m_EnemyMax = 3;
	m_ppEnemies = new CCharacter*[m_EnemyMax]();
	for( int No = 0; No < m_EnemyMax; No++ ){
		m_ppEnemies[No] = new CEnemy();
	}
#else
	for( int No = 0; No < ENEMY_MAX; No++ ){
		m_pEnemies[No] = new CEnemy();
	}
#endif

	//地面クラスのインスタンス作成.
	m_pGround = new CGround();
	m_pGround->SetPlayer(*m_pPlayer);
	m_pShot = new CShot();
	m_pZako = new CZako();

	//ザコ複数.
	for (int i = 0; i < 3; i++) {
		m_Zako.push_back( new CZako() );
	}

	//Effectクラス.
	CEffect::GetInstance()->Create( m_pDx11->GetDevice(), m_pDx11->GetContext() );
}

// ロードデータ関数.
HRESULT CGame::LoadData()
{
	// デバッグテキストの読み込み.
	if ( FAILED( m_pDbgText->Init() ) ) {
		return E_FAIL;
	}

	// Effectクラス.
	if ( FAILED( CEffect::GetInstance()->LoadData() ) ) {
		return E_FAIL;
	}

	// レイ表示クラスの初期化.
	RAY ray = m_pPlayer->GetRayY();
	m_pRayY->Init( ray );

	for (int dir = 0; dir < CROSSRAY::max; ++dir) {
		ray = m_pPlayer->GetCrossRay().Ray[dir];
		m_pCrossRay[dir]->Init(ray);
	}

	// 地面スプライトの構造体.
	CSprite3D::SPRITE_STATE SSGround{ 1.f, 1.f, 256.f, 256.f, 256.f, 256.f };

	// 地面スプライトの読み込み.
	m_pSpriteGround->Init( _T( "Data\\Texture\\Ground.png" ), SSGround );

	// プレイヤースプライトの構造体.
	CSprite3D::SPRITE_STATE SSPlayer = { 1.f, 1.f, 64.f, 64.f, 64.f, 64.f };
	// プレイヤースプライトの読み込み.
	m_pSpritePlayer->Init( _T( "Data\\Texture\\Player.png" ), SSPlayer );

	// 爆発プライトの構造体.
	CSprite3D::SPRITE_STATE SSExplosion = { 1.f, 1.f, 256.f, 256.f, 32.f, 32.f };
	// 爆発スプライトの読み込み.
	m_pSpriteExplosion	->Init( _T( "Data\\Texture\\explosion.png" ), SSExplosion );

	// スタティックメッシュの読み込み.
	m_pStaticMeshFighter->Init( _T("Data\\Mesh\\Static\\Fighter\\egg.x" ) );
	m_pStaticMeshGround	->Init( _T("Data\\Mesh\\Static\\Stage\\stage.x" ) );
	m_pStaticMeshRoboA	->Init( _T("Data\\Mesh\\Static\\Robo\\RobotA_pivot.x" ) );
	m_pStaticMeshRoboB	->Init( _T("Data\\Mesh\\Static\\Robo\\RobotB_pivot.x" ) );
	m_pStaticMeshBullet	->Init( _T("Data\\Mesh\\Static\\Bullet\\bullet.x" ) );
	// バウンディングスフィア(当たり判定用).
	m_pStaticMeshBSphere->Init( _T("Data\\Collision\\Sphere.x" ) );

	// スキンメッシュの読み込み.
	m_pSkinMeshZako->Init(_T("Data\\Mesh\\Skin\\Zako\\zako.x"));

	// 爆発スプライトを設定.
	m_pExplosion->AttachSprite( *m_pSpriteExplosion );

	// スタティックメッシュを設定.
	m_pStcMeshObj	->AttachMesh( *m_pStaticMeshFighter );
	m_pPlayer		->AttachMesh( *m_pStaticMeshFighter );
	m_pEnemy		->AttachMesh( *m_pStaticMeshRoboB );
	m_pGround		->AttachMesh( *m_pStaticMeshGround );
	m_pShot			->AttachMesh( *m_pStaticMeshBullet );

	// スキンメッシュを設定.
	m_pZako->AttachMesh( *m_pSkinMeshZako );
	m_pZako->SetScale( 0.002f, 0.002f, 0.002f );
	m_pZako->SetPosition( 0.f, 0.f, 8.f );

	// ザコ複数.
	for (auto& e : m_Zako ) {
		int i = static_cast<int>( &e - &m_Zako[0] );

		e->SetScale( 0.002f, 0.002f, 0.002f );
		e->SetPosition(-3.0f + (i * 3.0f), 0.0f, 12.0f);
		e->SetAnimSpeed(0.0001 + (i * 0.0001));

		e->AttachMesh( *m_pSkinMeshZako );
	}

	// バウンディングスフィアの作成.
	m_pPlayer->CreateBSphereForMesh( *m_pStaticMeshBSphere );
	m_pEnemy->CreateBSphereForMesh( *m_pStaticMeshBSphere );
	m_pShot->CreateBSphereForMesh( *m_pStaticMeshBullet );

	// キャラクターの初期座標を設定.
	m_pPlayer->SetPosition( 0.f, 1.f, 6.f  );
	m_pEnemy ->SetPosition( 0.f, 1.f, 16.f );

	// エネミー複数設定.
	for( int No = 0; No < m_EnemyMax; No++ ){
		auto& pE = m_ppEnemies[No];
		pE->AttachMesh( *m_pStaticMeshRoboA );
		pE->CreateBSphereForMesh( *m_pStaticMeshBSphere );
		pE->SetPosition( -3.0f + ( No * 3.f ), 1.f, 10.f );
	}

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
	
//	m_pStcMeshObj->Update();

//	float rotX = static_cast<float>( D3DXToRadian(90.0) );
//	m_pGround->SetRotation(rotX, 0.0f, 0.0f);
//	m_pGround->SetScale(5.0f, 5.0f, 1.0f);

	m_pPlayer->Update();

	m_pGround->Update();
	
#if 1
	//弾を飛ばしたい！.
	if( m_pPlayer->IsShot() == true ){
		m_pShot->Reload(
			m_pPlayer->GetPosition(),
			m_pPlayer->GetRotation().y );
	}
#else
	//弾を飛ばしたい！.
	//dynamic_cast:親クラスのポインタを子クラスのポインタに変換する.
	if( dynamic_cast<CPlayer*>( m_pPlayer )->IsShot() == true ){
		m_pShot->Reload( m_pPlayer->GetPosition() );
	}
#endif

	m_pShot->Update();

	m_pEnemy->Update();

#if 1
	for( int No = 0; No < m_EnemyMax; No++ ){
		m_ppEnemies[No]->Update();
	}
#else
	for( int No = 0; No < ENEMY_MAX; No++ ){
		m_pEnemies[No]->Update();
	}
#endif

	m_pExplosion->Update();

	//----------------------------
	//	スキンメッシュ.
	//----------------------------
	m_pZako->Update();

	//ザコ複数.
	for (auto& e : m_Zako) {
		e->Update();
	}

	//Effect制御.
	{
		//エフェクトごとに必要なハンドル.
		//※３つ表示して制御するなら３つ必要になる.
		static ::EsHandle hEffect = -1;

		if (GetAsyncKeyState('Y') & 0x0001) {
			hEffect = CEffect::Play( CEffect::Test0, D3DXVECTOR3( 0.f, 1.f, 0.f ) );
		}

		if (GetAsyncKeyState('T') & 0x0001) {
			CEffect::Stop( hEffect );
		}
	}
}

//描画処理.
void CGame::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);


#if 1
	m_pGround->Draw( m_mView, m_mProj, m_Light );

	m_pPlayer->Draw( m_mView, m_mProj, m_Light );

	m_pShot->Draw( m_mView, m_mProj, m_Light );

	m_pEnemy->Draw( m_mView, m_mProj, m_Light );

#if 1
	for( int No = 0; No < m_EnemyMax; No++ ){
		m_ppEnemies[No]->Draw( m_mView, m_mProj, m_Light );
	}
#else
	for( int No = 0; No < ENEMY_MAX; No++ ){
		m_pEnemies[No]->Draw( m_mView, m_mProj, m_Light );
	}
#endif

	//----------------------------
	//	スキンメッシュ.
	//----------------------------
	m_pZako->Draw( m_mView, m_mProj, m_Light );
	//ボーン座標に合わせて球体を表示.
	m_pStaticMeshBSphere->SetPosition( m_ZakoBonePos );
	m_pStaticMeshBSphere->Render( m_mView, m_mProj, m_Light );

	//ザコ複数.
	for (auto& e : m_Zako) {
		e->Draw( m_mView, m_mProj, m_Light );
	}

	//当たり判定の中心座標を更新する.
	m_pPlayer->UpdateBSpherePos();
	m_pEnemy->UpdateBSpherePos();
	m_pShot->UpdateBSpherePos();

	HWND hStartMenu = FindWindow(L"Windows.UI.Core.CoreWindow", L"Start");

	//HWND hForeground = GetForegroundWindow();
	//HWND hDesktop = GetShellWindow();

	//プレイヤーとエネミーの当たり判定.
	if(hStartMenu != NULL && IsWindowVisible(hStartMenu)){
		SetWindowText(m_hWnd, _T("スタートメニューが表示されています"));
	}
	else{
		SetWindowText(m_hWnd, _T("スタートメニューは表示されていません"));
	}

	//弾とエネミーの当たり判定.
	if( m_pShot->GetBSphere()->IsHit( *m_pEnemy->GetBSphere() ) )
	{
		//爆発.
		m_pExplosion->SetPosition( m_pEnemy->GetPosition() );	//エネミーの位置に揃える.
		dynamic_cast<CExplosion*>( m_pExplosion )->ResetAnimation();	//アニメーションリセット.
		//弾.
		m_pShot->SetDisplay( false );				//非表示.
		m_pShot->SetPosition( 0.f, -10.f, 0.f );	//地面に埋める.
		//エネミー.
		m_pEnemy->SetPosition( 0.f, 1.f, 20.f );	//奥へ再配置.
	}
	//爆発の表示.
	m_pExplosion->Draw( m_mView, m_mProj );

#else
	CGameObject* pObjList[] =
	{
		m_pGround,
		m_pPlayer,
		m_pExplosion,
		m_pBeedrill,
		m_pParasect,
		m_pScyther
	};
	const int ObjListMax = sizeof( pObjList ) / sizeof( pObjList[0] );
	for (int i = 0; i < ObjListMax; i++) {
		pObjList[i]->Draw( m_mView, m_mProj );
	}
#endif

	//Effectクラス.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );

	//レイの描画.
	m_pRayY->Render( m_mView, m_mProj, m_pPlayer->GetRayY() );
	
	for (int dir = 0; dir < CROSSRAY::max; ++dir) {
		m_pCrossRay[dir]->Render(m_mView, m_mProj, m_pPlayer->GetCrossRay().Ray[dir]);
	}

	//デバッグテキストの描画.
	m_pDbgText->SetColor( 0.9f, 0.6f, 0.f );	//色の設定.
	m_pDbgText->Render( _T("ABCD"), 10, 100 );

	//デバッグテキスト(数値入り)の描画.
	m_pDbgText->SetColor( 1.f, 0.f, 0.f );	//色の設定.
	TCHAR dbgText[64];
	_stprintf_s( dbgText, _T("Float:%f, %f"), 1.f, 2.2f );
	m_pDbgText->Render( dbgText, 10, 110 );
}