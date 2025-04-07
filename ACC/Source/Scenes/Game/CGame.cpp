#include "CGame.h"

#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectInput/CDirectInput.h"

#include "Effect/CEffect.h"

#include "Camera/CCamera.h"
#include "Character/Player/CPlayer.h"
#include "Character/Enemy/CEnemy.h"
#include "Sprite/2D/UI/CGameUI/CGameUI.h"
#include "Sprite/2D/UI/CLoseUI/CLoseUI.h"
#include "Sprite/2D/UI/CWinUI/CWinUI.h"
#include "Random/CRandom.h"


//============================================================================
//		ゲームクラス.
//============================================================================
CGame::CGame(HWND hWnd)
	: m_hWnd		( hWnd )
	, m_mView		()
	, m_mProj		()
	, m_Light		()

	, m_pEgg		( nullptr )
	, m_pFloor		( nullptr )
	, m_pCylinders	()
	
	, m_pPlayer		( nullptr )
	, m_pEnemy		( nullptr )

	, m_pGJK		( nullptr )
	, m_pCamRay		( nullptr )

	, m_pWinUI		( nullptr )
	, m_pLoseUI		( nullptr )

	, m_HitKind		( 0 )
	, m_CylinderMax	( 9 )
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
	// インスタンス生成.
	m_pEgg			= std::make_unique<CStaticMesh>();
	m_pFloor		= std::make_unique<CStaticMesh>();
	m_pPlayer		= std::make_unique<CPlayer>();
	m_pEnemy		= std::make_unique<CEnemy>();
	m_pCamRay		= std::make_unique<CRay>();
	m_pGameUI		= std::make_unique<CGameUI>();
	m_pGameUI		->Create();

	// スポーン地点の設定.
	m_SpawnPoints = {
		{  50.f, 5.f,  50.f },
		{ -50.f, 5.f,  50.f },
		{  50.f, 5.f, -50.f },
		{ -50.f, 5.f, -50.f }
	};
	
	// 柱の生成.
	for (int i = 0; i < m_CylinderMax; ++i) {
		auto cylinder = std::make_unique<CStaticMesh>();
		cylinder->Init(_T("Data\\Mesh\\Static\\Stage\\Rectangular.x"));
		m_pCylinders.push_back(std::move(cylinder));
	}
}


//============================================================================
//		データ読込.
//============================================================================
HRESULT CGame::LoadData()
{
	// スタティックメッシュの読み込み.
	m_pEgg		->Init( _T("Data\\Mesh\\Static\\Player\\egg.x"	));
	m_pFloor	->Init( _T("Data\\Mesh\\Static\\Stage\\Floor.x"	));

	// メッシュをアタッチする.
	m_pPlayer	->AttachMesh( *m_pEgg );
	m_pEnemy	->AttachMesh( *m_pEgg );

	// キャラクターの初期座標を設定.
	m_pPlayer	->SetScale( 2.f, 2.f, 2.f );
	m_pEnemy	->SetScale( 2.f, 2.f, 2.f );

	// プレイヤーと敵のスポーン位置をランダムに決める.
	CRandom random;
	InitEPPos(random, m_pPlayer, m_pEnemy);

	// 柱の配置(後でどうにかする仮設置方法).
	for (int i = 0; i < m_pCylinders.size(); ++i)
	{
		// ↓外壁.
		if (i == 0) {
			m_pCylinders[i]->SetPos(0.f, -10.f, 103.f);
			m_pCylinders[i]->SetScale(50.f, 0.5f, 1.f);
		}
		if (i == 1) {
			m_pCylinders[i]->SetPos(0.f, -10.f, -103.f);
			m_pCylinders[i]->SetScale(50.f, 0.5f, 1.f);
		}
		if (i == 2) {
			m_pCylinders[i]->SetPos(103.f, -10.f, 0.f);
			m_pCylinders[i]->SetScale(1.f, 0.5f, 50.f);
		}
		if (i == 3) {
			m_pCylinders[i]->SetPos(-103.f, -10.f, 0.f);
			m_pCylinders[i]->SetScale(1.f, 0.5f, 50.f);
		}
		// ↑外壁.

		// ↓柱.
		if (i == 4) {
			m_pCylinders[i]->SetPos(0.f, -0.3f, 0.f);
			m_pCylinders[i]->SetScale(5.f, 0.5f, 5.f);
		}
		if (i == 5) {
			m_pCylinders[i]->SetPos(51.5f, -0.3f, 0.f);
			m_pCylinders[i]->SetScale(5.f, 0.5f, 5.f);
		}
		if (i == 6) {
			m_pCylinders[i]->SetPos(-51.5f, -0.3f, 0.f);
			m_pCylinders[i]->SetScale(5.f, 0.5f, 5.f);
		}
		if (i == 7) {
			m_pCylinders[i]->SetPos(0.f, -0.3f, 51.5f);
			m_pCylinders[i]->SetScale(5.f, 0.5f, 5.f);
		}
		if (i == 8) {
			m_pCylinders[i]->SetPos(0.f, -0.3f, -51.5f);
			m_pCylinders[i]->SetScale(5.f, 0.5f, 5.f);
		}
		// ↑柱.
	}

	// カメラの初期化.
	CCamera::GetInstance()->Init();

	// カメラのレイ情報を取得.
	m_pCamRay->Init(CCamera::GetInstance()->GetRay());

	return S_OK;
}


//============================================================================
//		解放関数.
//============================================================================
void CGame::Release()
{
	m_pGameUI.reset();
	m_pCamRay.reset();
	m_pEnemy.reset();
	m_pPlayer.reset();
	for (auto& cylinder : m_pCylinders) {
		cylinder.reset();
	}
	m_pEgg.reset();
	m_pFloor.reset();

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
	// BGMを再生.
	CSoundManager::GetInstance()->PlayLoop(CSoundManager::enList::BGM_Game);

	CKey* Key = CInput::GetInstance()->CDKeyboard();

	// UIの更新処理.
	UIUpdate();

	// 勝利や敗北画面が出現していない間.
	if (m_pLoseUI == nullptr && m_pWinUI == nullptr)
	{
		// その他とカメラレイの判定(弾の到着地点に使用する).
		RaytoObjeCol();

		m_pPlayer->Update(m_pEnemy);		// プレイヤーの更新.
		m_pEnemy->Update(m_pPlayer);		// エネミーの更新.
		CCamera::GetInstance()->Update();	// カメラの更新.

		// プレイヤーがオートエイムを使用していた場合.
		if (m_pPlayer->GetAutoAim()) {
			CCamera::GetInstance()->SetLook(m_pEnemy->GetPos());
		}

		// 当たり判定処理.
		CollisionJudge();

		// UIの更新処理.
		m_pGameUI->Update(m_pPlayer);
	}

	// カメラ側のキー操作を無効にする.
	if (Key->IsKeyAction(DIK_F3)) { CCamera::GetInstance()->ChangeUseMouse(); }
}


//============================================================================
//		描画処理.
//============================================================================
void CGame::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);

	m_pFloor	->Render( m_mView, m_mProj, m_Light );	// 地面の描画.
	m_pPlayer	->Draw( m_mView, m_mProj, m_Light );	// プレイヤーの描画.
	m_pEnemy	->Draw( m_mView, m_mProj, m_Light );	// 敵の描画.

	// 柱の描画.
	for (auto& cylinder : m_pCylinders) {
		cylinder->Render(m_mView, m_mProj, m_Light);
	}

	// エフェクトの描画.
	CEffect::GetInstance()->Draw(m_mView, m_mProj, m_Light);

	// UIの描画.
	m_pGameUI->Draw();


	// 勝利と敗北画面の描画.
	if (m_pLoseUI != nullptr) { m_pLoseUI->Draw(); }
	if (m_pWinUI != nullptr) { m_pWinUI->Draw(); }
}


//-----------------------------------------------------------------------------
//		当たり判定関数.
//-----------------------------------------------------------------------------
void CGame::CollisionJudge()
{
	Collider PlayerEgg, EnemyEgg, Floor, Cylinder;
	std::vector<Collider> Cylinders;

	// 柱データ取得.
	for (int i = 0; i < m_CylinderMax; ++i) {
		Cylinder.SetVertex( m_pCylinders[i]->GetObjeInfo(), m_pCylinders[i]->GetVertices());
		Cylinders.push_back(Cylinder);
	}

	// 地面,プレイヤー,敵のデータ取得.
	Floor		.SetVertex( m_pFloor->GetObjeInfo(),	m_pFloor->GetVertices());
	PlayerEgg	.SetVertex( m_pPlayer->GetObjeInfo(),	m_pEgg->GetVertices());
	EnemyEgg	.SetVertex( m_pEnemy->GetObjeInfo(),	m_pEgg->GetVertices());

	// プレイヤーと敵の柱判定を取得用の変数を用意.
	CollisionPoints pointsPC, pointsEC;

	// プレイヤーと円柱の判定を返す.
	std::vector<CollisionPoints> pointspecs;
	for (int i = 0; i < m_CylinderMax; ++i) {
		pointsPC = m_pGJK->GJK(Cylinders[i], PlayerEgg);
		pointspecs.push_back(pointsPC);
	}

	// 敵と円柱の判定を返す.
	std::vector<CollisionPoints> pointseecs;
	for (int i = 0; i < m_CylinderMax; ++i) {
		pointsEC = m_pGJK->GJK(Cylinders[i], EnemyEgg);
		pointseecs.push_back(pointsEC);
	}

	// プレイヤーと敵の床の判定を取得.
	CollisionPoints pointsPF = m_pGJK->GJK(PlayerEgg, Floor), pointsEF = m_pGJK->GJK(EnemyEgg, Floor);

	// プレイヤーと敵の床の衝突判定処理.
	PlayertoFloorCol(pointsPF);
	EnemytoFloorCol(pointsEF);

	// プレイヤーと敵に重力を加える.
	m_pPlayer->UseGravity();
	m_pEnemy->UseGravity();

	// プレイヤーと敵の柱の衝突判定処理.
	for (int i = 0; i < m_CylinderMax; ++i) { PlayertoCylinderCol(pointspecs[i]); }
	for (int i = 0; i < m_CylinderMax; ++i) { EnemytoCylinderCol(pointseecs[i]); }

	// プレイヤーと敵の当たり判定処理をする.
	for (int i = 0; i < m_CylinderMax; ++i) {
		m_pPlayer->Collision(m_pEnemy, Floor, Cylinders[i]);
		m_pEnemy->Collision(m_pPlayer, Floor, Cylinders[i]);
	}
}


//-----------------------------------------------------------------------------
//		敵とプレイヤーをンダムでスポーン.
//-----------------------------------------------------------------------------
void CGame::InitEPPos(CRandom& random, std::unique_ptr<CPlayer>& player, std::unique_ptr<CEnemy>& enemy)
{
	// スポーンポイントのインデックスをランダムに取得.
	int PIndex = random.GetRandomInt(0, m_SpawnPoints.size() - 1);

	// 敵のスポーンポイントをプレイヤーと異なる場所にする.
	int EIndex;
	do {
		EIndex = random.GetRandomInt(0, m_SpawnPoints.size() - 1);
	} while (EIndex == PIndex);

	// プレイヤーと敵の位置を設定.
	player->SetPos(m_SpawnPoints[PIndex]);
	enemy->SetPos(m_SpawnPoints[EIndex]);
}


//-----------------------------------------------------------------------------
//		プレイヤーと床の当たり判定をまとめる関数.
//-----------------------------------------------------------------------------
void CGame::PlayertoFloorCol(CollisionPoints points)
{
	// 当たっていた場合.
	if (points.Col)
	{
		m_pPlayer->ResetGravity();	// プレイヤーにかかる重力のリセット.
		m_pPlayer->CanJump();		// ジャンプを可能に.

		// 地面に衝突している場合.
		if (points.Normal.y < 0.f)
		{
			// プレイヤーを押し戻したときの座標を算出.
			D3DXVECTOR3 SetPos = m_pPlayer->GetPos() - points.Normal * points.Depth;

			// プレイヤーの位置の更新.
			m_pPlayer->SetPos(SetPos);
		}
		// 壁や斜面に衝突している場合.
		else {
			D3DXVec3Normalize(&points.Normal, &points.Normal); // 正規化.
			D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();  // プレイヤーの移動ベクトルを取得.
			
			// プレイヤーの移動ベクトルと法線ベクトルの内積を計算.
			float Dot = D3DXVec3Dot(&PlayerMove, &points.Normal);

			// 法線方向の移動成分を除去して、壁に沿った移動成分のみにする.
			PlayerMove = PlayerMove - Dot * points.Normal;

			// 修正した移動ベクトルをプレイヤーに加算.
			m_pPlayer->AddVec(PlayerMove);
		}
	}
	else {
		m_pPlayer->AddGravity();	// プレイヤーにかかる重力を増やす.
		m_pPlayer->JumpPowerDec();	// ジャンプで加算される値を減らす.
	}
}


//-----------------------------------------------------------------------------
//		プレイヤーと柱の当たり判定をまとめる関数.
//-----------------------------------------------------------------------------
void CGame::PlayertoCylinderCol(CollisionPoints points)
{
	if (points.Col) {
		D3DXVec3Normalize(&points.Normal, &points.Normal);	// 法線を正規化.
		D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();	// プレイヤーの移動ベクトルを取得.

		// プレイヤーの移動ベクトルと法線ベクトルの内積を計算.
		float Dot = D3DXVec3Dot(&PlayerMove, &points.Normal);

		// 法線方向の移動成分を除去して、壁に沿った移動成分のみにする.
		PlayerMove = PlayerMove - Dot * points.Normal;

		// 修正した移動ベクトルをプレイヤーに適用.
		m_pPlayer->AddVec(PlayerMove);
		m_pPlayer->SetPos(m_pPlayer->GetPos() + points.Normal * points.Depth);

		// 深度が設定値以下の場合.
		if (points.Depth < 0.05f) {
			m_pPlayer->AddVec(-m_pPlayer->GetMoveVec());
		}
	}
}


//-----------------------------------------------------------------------------
//		敵と床の当たり判定をまとめる関数.
//-----------------------------------------------------------------------------
void CGame::EnemytoFloorCol(CollisionPoints points)
{
	// 当たっていた場合.
	if (points.Col)
	{
		m_pEnemy->ResetGravity();	// 敵にかかる重力のリセット.
		m_pEnemy->CanJump();		// ジャンプを可能に.

		// 地面に衝突している場合.
		if (points.Normal.y < 0.f)
		{
			// 敵を押し戻したときの座標を算出.
			D3DXVECTOR3 SetPos = m_pEnemy->GetPos() - points.Normal * points.Depth;

			// 敵の位置の更新.
			m_pEnemy->SetPos(SetPos);
		}
		// 壁や斜面に衝突している場合.
		else {
			D3DXVec3Normalize(&points.Normal, &points.Normal);	// 正規化.
			D3DXVECTOR3 EnemyMove = m_pEnemy->GetMoveVec();		// 敵の移動ベクトルを取得.
			
			// 敵の移動ベクトルと法線ベクトルの内積を計算.
			float Dot = D3DXVec3Dot(&EnemyMove, &points.Normal);

			// 法線方向の移動成分を除去して、壁に沿った移動成分のみにする.
			EnemyMove = EnemyMove - Dot * points.Normal;

			// 修正した移動ベクトルを敵に加算.
			m_pEnemy->AddVec(EnemyMove);
		}
	}
	else {
		m_pEnemy->AddGravity();	 // 敵にかかる重力を増やす.
		m_pEnemy->JumpPowerDec();// ジャンプで加算される値を減らす.
	}
}


//-----------------------------------------------------------------------------
//		敵と柱の当たり判定をまとめる関数.
//-----------------------------------------------------------------------------
void CGame::EnemytoCylinderCol(CollisionPoints points)
{
	// 当たっている場合.
	if (points.Col) {
		D3DXVec3Normalize(&points.Normal, &points.Normal);	// 法線を正規化.
		D3DXVECTOR3 EnemyMove = m_pEnemy->GetMoveVec();		// プレイヤーの移動ベクトルを取得.

		// プレイヤーの移動ベクトルと法線ベクトルの内積を計算.
		float Dot = D3DXVec3Dot(&EnemyMove, &points.Normal);

		// 法線方向の移動成分を除去して、壁に沿った移動成分のみにする.
		EnemyMove = EnemyMove - Dot * points.Normal;

		// 修正した移動ベクトルを敵に適用.
		m_pEnemy->AddVec(EnemyMove);
		m_pEnemy->SetPos(m_pEnemy->GetPos() + points.Normal * points.Depth);

		// 深度が設定値以下の場合.
		if (points.Depth < 0.05f) {
			m_pEnemy->AddVec(-m_pEnemy->GetMoveVec());
		}
	}
}


//-----------------------------------------------------------------------------
//		レイの当たり判定をまとめる関数.
//-----------------------------------------------------------------------------
void CGame::RaytoObjeCol()
{
	// カメラベクトル.
	D3DXVECTOR3 camlookpos = CCamera::GetInstance()->GetPos() + CCamera::GetInstance()->GetCamDir() * 100.f;
	
	// レイ情報用の変数.
	RayInfo SendCamera = { false, camlookpos, 5000.f }, GroundRay, EnemyRay, CylinderRay;
	std::vector<RayInfo> CylinderRays;	// 柱用のレイ配列.

	//-------------------------------------------------------------------------
	//	カメラレイと各オブジェごとの判定情報を取得.
	//-------------------------------------------------------------------------
	
	// 地面とカメラレイの判定を取得.
	GroundRay = m_pFloor->IsHitForRay(CCamera::GetInstance()->GetRay());
	
	// 柱とカメラレイの判定を取得.
	for (int i = 0; i < m_CylinderMax; ++i) {
		CylinderRay = m_pCylinders[i]->IsHitForRay(CCamera::GetInstance()->GetRay());
		CylinderRays.push_back(CylinderRay);
	}
	
	// 敵とカメラレイの判定を取得.
	EnemyRay = m_pEnemy->IsHitForRay(CCamera::GetInstance()->GetRay());


	//-------------------------------------------------------------------------
	//		どのオブジェクトが最もカメラから近いかを探す.
	//-------------------------------------------------------------------------
	
	// レイが地面にあたっている場合.
	if (GroundRay.Hit) {
		// 地面からカメラまでのレイ長さを比較、trueの場合値を上書きする.
		if (SendCamera.Length > GroundRay.Length) { SendCamera = GroundRay; }
	}
	
	// レイが敵にあたっている場合.
	if (EnemyRay.Hit) {
		// 敵からカメラまでのレイ長さを比較し、trueの場合値を上書きする.
		if (SendCamera.Length > EnemyRay.Length) { SendCamera = EnemyRay; }
	}

	// 柱の数分forループを回す.
	for (int i = 0; i < m_CylinderMax; ++i) {
		// レイが柱にあたっている場合.
		if (CylinderRays[i].Hit) {
			// 柱からカメラまでのレイ長さを比較、trueの場合値を上書きする.
			if (SendCamera.Length > CylinderRays[i].Length) { SendCamera = CylinderRays[i]; }
		}
	}

	// 最終的に最短距離にあるオブジェクトのHit座標を渡す.
	CCamera::GetInstance()->SetRayHit(SendCamera.HitPos);
}


//-----------------------------------------------------------------------------
//		UI処理をまとめる関数.
//-----------------------------------------------------------------------------
void CGame::UIUpdate()
{
	// プレイヤーがHP０の場合、もしくは地面抜けしていた場合、勝利UIを作成.
	if (m_pLoseUI == nullptr
	&& (m_pPlayer->GetCharaInfo().HP <= 0|| m_pPlayer->GetPos().y < -100.f)) 
	{
		// 敗北UIの作成
		m_pLoseUI = std::make_unique<CLoseUI>();
		m_pLoseUI->Create();

		// 敗北時の効果音を鳴らす.
		CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_Lose);
	}

	// 敵がHP０の場合、もしくは地面抜けしていた場合、勝利UIを作成.
	if (m_pWinUI == nullptr 
	&& (m_pEnemy->GetCharaInfo().HP <= 0|| m_pEnemy->GetPos().y < -100.f)) 
	{
		// 勝利UIの作成
		m_pWinUI = std::make_unique<CWinUI>();
		m_pWinUI->Create();

		// 勝利時の効果音を鳴らす.
		CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_Win);
	}
	
	// 勝利や敗北画面の更新処理.
	if ( m_pLoseUI != nullptr )	{ m_pLoseUI->Update();	}
	if ( m_pWinUI  != nullptr )	{ m_pWinUI->Update();	}
}