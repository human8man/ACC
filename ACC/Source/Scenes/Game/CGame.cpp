#include "CGame.h"

#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectInput/CDirectInput.h"

#include "Effect/CEffect.h"

#include "Camera/CCamera.h"
#include "Object/Ground/CGround.h"
#include "Character/Player/CPlayer.h"
#include "Character/Enemy/CEnemy.h"
#include "Sprite/2D/UI/CGameUI/CGameUI.h"

#include "Common/Random/CRandom.h"

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

	, m_pEgg		( nullptr )
	, m_pFloor		( nullptr )
	, m_pCylinders	()
	
	, m_pPlayer		( nullptr )
	, m_pEnemy		( nullptr )
	, m_pGround		( nullptr )

	, m_pGJK		( nullptr )
	, m_pCamRay		( nullptr )

	, m_Angle		(0.f)
	, m_CylinderMax	(9)
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
	m_pEgg		= std::make_unique<CStaticMesh>();
	m_pFloor	= std::make_unique<CStaticMesh>(); 
	for (int i = 0; i < m_CylinderMax; ++i) {
		auto cylinder = std::make_unique<CStaticMesh>();
		cylinder->Init(_T("Data\\Mesh\\Static\\Stage\\Rectangular.x"));
		m_pCylinders.push_back(std::move(cylinder));
	}
	m_pPlayer	= std::make_unique<CPlayer>();
	m_pEnemy	= std::make_unique<CEnemy>();
	m_pGround	= std::make_unique<CGround>();
	m_pCamRay	= std::make_unique<CRay>();
	m_pGameUI	= std::make_unique<CGameUI>();
	m_pGameUI	->Create();

	// スポーン地点の設定.
	m_SpawnPoints = {
		{  50.f, 5.f,  50.f },
		{ -50.f, 5.f,  50.f },
		{  50.f, 5.f, -50.f },
		{ -50.f, 5.f, -50.f }
	};
}


//============================================================================
//		ロードデータ関数.
//============================================================================
HRESULT CGame::LoadData()
{
	// スタティックメッシュの読み込み.
	m_pEgg		->Init( _T("Data\\Mesh\\Static\\Player\\egg.x"	));
	m_pFloor	->Init( _T("Data\\Mesh\\Static\\Stage\\Floor.x"	));

	// メッシュをアタッチする.
	m_pPlayer	->AttachMesh( *m_pEgg );
	m_pEnemy	->AttachMesh( *m_pEgg );
	m_pGround	->AttachMesh( *m_pFloor );

	// キャラクターの初期座標を設定.
	m_pPlayer	->SetPos( 60.f, 50.f, 60.f );
	m_pEnemy	->SetPos( 10.f, 1.f, 6.f );

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
	m_pGround.reset();
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
	CKey* Key = CDInput::GetInstance()->CDKeyboard();

	// その他とカメラレイの判定(弾の到着地点に使用する).
	RaytoObjeCol();

	m_pPlayer->Update(); // プレイヤーの更新.
	m_pEnemy->Update();	 // エネミーの更新.
	CCamera::GetInstance()->Update(); // カメラの更新.


	// 下二つには絶対に演出を入れる（勝利,敗北演出).
	// プレイヤーのHPが０になったとき(バグった時).
	if( m_pPlayer->GetCharaInfo().HP <= 0 
	||  m_pPlayer->GetPos().y < -100.f) {
		CSceneManager::GetInstance()->LoadScene(SceneList::Title);
	}

	// 敵のHPが０になったとき(バグった時).
	if( m_pEnemy->GetCharaInfo().HP <= 0
	||  m_pEnemy->GetPos().y < -100.f) {
		CSceneManager::GetInstance()->LoadScene(SceneList::Title);
	}

	CollisionJudge();

	m_pGameUI->Update();

#if _DEBUG
	//-----------------------------------------------------
	//	キャラクターウィンドウ.
	//-----------------------------------------------------
	ImGui::Begin("CharaWindow");
	ImGui::Text("%d", m_pPlayer->GetCharaInfo().HP);
	ImGui::Text("%d", m_pEnemy->GetCharaInfo().HP);
	ImGui::End();


	//-----------------------------------------------------
	//	プレイヤーウィンドウ.
	//-----------------------------------------------------
	ImGui::Begin("PlayerWindow");
	D3DXVECTOR3 playerpos = m_pPlayer->GetPos();
	ImGui::Text("%f,%f,%f", playerpos.x, playerpos.y, playerpos.z);
	D3DXVECTOR3 playersumvec = m_pPlayer->GetMoveVec();
	ImGui::Text("%f,%f,%f", playersumvec.x, playersumvec.y, playersumvec.z);
	ImGui::End();


	//-----------------------------------------------------
	//	銃ウィンドウ.
	//-----------------------------------------------------
	ImGui::Begin("CameraWindow");
	D3DXVECTOR3 camrot = CCamera::GetInstance()->GetRot();
	ImGui::Text("%f,%f,%f", camrot.x, camrot.y, camrot.z);
	ImGui::End();


	// カメラ側のキー操作を無効にする.
	if (Key->IsKeyAction(DIK_F2)) { CCamera::GetInstance()->ChangeCanMove(); }
	if (Key->IsKeyAction(DIK_F3)) { CCamera::GetInstance()->ChangeUseMouse(); }
#endif
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
	m_pEnemy->Draw( m_mView, m_mProj, m_Light );

	for (auto& cylinder : m_pCylinders) {
		cylinder->Render(m_mView, m_mProj, m_Light);
	}
	m_pGameUI->Draw();

	// エフェクトの描画.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );
}


//-----------------------------------------------------------------------------
//		当たり判定関数.
//-----------------------------------------------------------------------------
void CGame::CollisionJudge()
{
	MeshCollider PlayerEgg, EnemyEgg, Floor;
	std::vector<MeshCollider> Cylinders;

	// 柱データ取得.
	for (int i = 0; i < m_CylinderMax; ++i) {
		MeshCollider cylinder;
		cylinder.SetVertex(
			m_pCylinders[i]->GetPos(),
			m_pCylinders[i]->GetRot(),
			m_pCylinders[i]->GetScale(),
			m_pCylinders[i]->GetVertices()
		);
		Cylinders.push_back(cylinder);
	}

	// 地面データ取得.
	Floor.SetVertex(
		m_pGround->GetPos(),
		m_pGround->GetRot(),
		m_pGround->GetScale(),
		m_pFloor->GetVertices());

	// プレイヤーデータ取得.
	PlayerEgg.SetVertex(
		m_pPlayer->GetPos(),
		m_pPlayer->GetRot(),
		m_pPlayer->GetScale(),
		m_pEgg->GetVertices());
	
	// プレイヤーデータ取得.
	EnemyEgg.SetVertex(
		m_pEnemy->GetPos(),
		m_pEnemy->GetRot(),
		m_pEnemy->GetScale(),
		m_pEgg->GetVertices());


	// プレイヤーと床の判定を返す.
	CollisionPoints pointspef = m_pGJK->GJK(PlayerEgg, Floor);
	// プレイヤーと円柱の判定を返す.
	std::vector<CollisionPoints> pointspecs;
	for (int i = 0; i < m_CylinderMax; ++i) {
		CollisionPoints pointspec;
		pointspec = m_pGJK->GJK(Cylinders[i], PlayerEgg);
		pointspecs.push_back(pointspec);
	}

	// 敵と床の判定を返す.
	CollisionPoints pointseef = m_pGJK->GJK(EnemyEgg, Floor);
	// 敵と円柱の判定を返す.
	std::vector<CollisionPoints> pointseecs;
	for (int i = 0; i < m_CylinderMax; ++i) {
		CollisionPoints pointseec;
		pointseec = m_pGJK->GJK(Cylinders[i], EnemyEgg);
		pointseecs.push_back(pointseec);
	}


	// プレイヤーと床の判定処理.
	PlayertoFloorCol(pointspef);

	// プレイヤーに重力を加える.
	m_pPlayer->UseGravity();

	// プレイヤーと柱の判定処理.
	for (int i = 0; i < m_CylinderMax; ++i) {
		PlayertoCylinderCol(pointspecs[i]);
	}

	// エネミーと床の判定処理.
	EnemytoFloorCol(pointseef);

	// エネミーに重力を加える.
	m_pEnemy->UseGravity();

	// エネミーと柱の判定処理.
	for (int i = 0; i < m_CylinderMax; ++i) {
		EnemytoCylinderCol(pointseecs[i]);
	}

	// プレイヤーと敵の当たり判定処理をする.
	for (int i = 0; i < m_CylinderMax; ++i) {
		m_pPlayer->Collision(m_pEnemy, Floor, Cylinders[i]);
		m_pEnemy->Collision(m_pPlayer, Floor, Cylinders[i]);
	}
}


//-----------------------------------------------------------------------------
//	ランダムでスポーン
//-----------------------------------------------------------------------------
void CGame::InitEPPos(CRandom& random, std::unique_ptr<CPlayer>& player, std::unique_ptr<CEnemy>& enemy) {
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
		D3DXVec3Normalize(&points.Normal, &points.Normal); // 法線を正規化.
		D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();  // プレイヤーの移動ベクトルを取得.

		// プレイヤーの移動ベクトルと法線ベクトルの内積を計算.
		float Dot = D3DXVec3Dot(&PlayerMove, &points.Normal);
		// 法線方向の移動成分を除去して、壁に沿った移動成分のみにする.
		PlayerMove = PlayerMove - Dot * points.Normal;

		// 修正した移動ベクトルをプレイヤーに適用.
		m_pPlayer->AddVec(PlayerMove);
		m_pPlayer->SetPos(m_pPlayer->GetPos() + points.Normal * points.Depth);

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
		m_pEnemy->ResetGravity();	// プレイヤーにかかる重力のリセット.
		m_pEnemy->CanJump();		// ジャンプを可能に.

		// 地面に衝突している場合.
		if (points.Normal.y < 0.f)
		{
			// プレイヤーを押し戻したときの座標を算出.
			D3DXVECTOR3 SetPos = m_pEnemy->GetPos() - points.Normal * points.Depth;

			// プレイヤーの位置の更新.
			m_pEnemy->SetPos(SetPos);
		}
		// 壁や斜面に衝突している場合.
		else {
			D3DXVec3Normalize(&points.Normal, &points.Normal); // 正規化.
			D3DXVECTOR3 PlayerMove = m_pEnemy->GetMoveVec();   // プレイヤーの移動ベクトルを取得.
			
			// プレイヤーの移動ベクトルと法線ベクトルの内積を計算.
			float Dot = D3DXVec3Dot(&PlayerMove, &points.Normal);

			// 法線方向の移動成分を除去して、壁に沿った移動成分のみにする.
			PlayerMove = PlayerMove - Dot * points.Normal;

			// 修正した移動ベクトルをプレイヤーに加算.
			m_pEnemy->AddVec(PlayerMove);
		}
	}
	else {
		m_pEnemy->AddGravity();	 // プレイヤーにかかる重力を増やす.
		m_pEnemy->JumpPowerDec();// ジャンプで加算される値を減らす.
	}
}


//-----------------------------------------------------------------------------
//		敵と柱の当たり判定をまとめる関数.
//-----------------------------------------------------------------------------
void CGame::EnemytoCylinderCol(CollisionPoints points)
{
	if (points.Col) {
		D3DXVec3Normalize(&points.Normal, &points.Normal); // 法線を正規化.
		D3DXVECTOR3 PlayerMove = m_pEnemy->GetMoveVec();  // プレイヤーの移動ベクトルを取得.

		// プレイヤーの移動ベクトルと法線ベクトルの内積を計算.
		float Dot = D3DXVec3Dot(&PlayerMove, &points.Normal);
		// 法線方向の移動成分を除去して、壁に沿った移動成分のみにする.
		PlayerMove = PlayerMove - Dot * points.Normal;

		// 修正した移動ベクトルをプレイヤーに適用.
		m_pEnemy->AddVec(PlayerMove);
		m_pEnemy->SetPos(m_pPlayer->GetPos() + points.Normal * points.Depth);

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
	// レイ情報用の変数.
	RayInfo SendCamera, GroundRay, EnemyRay;
	std::vector<RayInfo> CylinderRays;

	D3DXVECTOR3 camlookpos = CCamera::GetInstance()->GetPos() + CCamera::GetInstance()->GetCamDir() * 100.f;
	SendCamera = { false, camlookpos, 5000.f };

	// カメラレイと各オブジェごとの判定情報を取得.
	GroundRay	= m_pGround->IsHitForRay(CCamera::GetInstance()->GetRay());
	for (int i = 0; i < m_CylinderMax; ++i) {
		RayInfo CylinderRay;
		CylinderRay = m_pCylinders[i]->IsHitForRay(CCamera::GetInstance()->GetRay());
		CylinderRays.push_back(CylinderRay);
	}
	EnemyRay = m_pEnemy->IsHitForRay(CCamera::GetInstance()->GetRay());


	// どのオブジェが最も近いかを探す.
	if (GroundRay.Hit) {
		if (SendCamera.Length > GroundRay.Length) {
			SendCamera = GroundRay;
		}
	}
	if (EnemyRay.Hit) {
		if (SendCamera.Length > EnemyRay.Length) {
			SendCamera = EnemyRay;
		}
	}
	for (int i = 0; i < m_CylinderMax; ++i) {
		if (CylinderRays[i].Hit) {
			if (SendCamera.Length > CylinderRays[i].Length) {
				SendCamera = CylinderRays[i];
			}
		}
	}

	// 最終的に最短距離にある物のあたった座標を渡す.
	CCamera::GetInstance()->SetRayHit(SendCamera.HitPos);
}