#include "CGame.h"

#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectInput/CDirectInput.h"

#include "Effect/CEffect.h"

#include "Camera/CCamera.h"
#include "Object/Ground/CGround.h"
#include "Character/Player/CPlayer.h"
#include "Character/Enemy/CEnemy.h"

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
	, m_pCylinder	( nullptr )
	
	, m_pPlayer		( nullptr )
	, m_pEnemy		( nullptr )
	, m_pGround		( nullptr )

	, m_pGJK		( nullptr )
	, m_pCamRay		( nullptr )

	, m_Angle		(0.f)
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
	m_pCylinder = std::make_unique<CStaticMesh>();
	m_pPlayer	= std::make_unique<CPlayer>();
	m_pEnemy	= std::make_unique<CEnemy>();
	m_pGround	= std::make_unique<CGround>();
	m_pCamRay	= std::make_unique<CRay>();
}

//============================================================================
//		ロードデータ関数.
//============================================================================
HRESULT CGame::LoadData()
{
	// スタティックメッシュの読み込み.
	m_pEgg		->Init( _T("Data\\Mesh\\Static\\Player\\egg.x"			));
	m_pFloor	->Init( _T("Data\\Mesh\\Static\\Stage\\Floor.x"			));
	m_pCylinder	->Init( _T("Data\\Mesh\\Static\\Stage\\Rectangular.x"	));

	// メッシュをアタッチする.
	m_pPlayer->AttachMesh( *m_pEgg );
	m_pEnemy->AttachMesh( *m_pEgg );
	m_pGround->AttachMesh( *m_pFloor );

	// キャラクターの初期座標を設定.
	m_pPlayer->SetPos( 0.f, 1.f, 6.f );
	m_pEnemy->SetPos( 10.f, 1.f, 6.f );
	m_pCylinder->SetPos( 10.f, -0.3f, 10.f );

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
	m_pCamRay.reset();
	m_pGround.reset();
	m_pEnemy.reset();
	m_pPlayer.reset();
	m_pCylinder.reset();
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
	CKey*	Key = CDInput::GetInstance()->CDKeyboard();

	// プレイヤーの更新.
	m_pPlayer->Update();

	// カメラの更新.
	CCamera::GetInstance()->Update();

	

	// 下二つには絶対に演出を入れる（勝利,敗北演出).
	// プレイヤーのHPが０になったとき.
	if(m_pPlayer->GetCharaInfo().HP == 0)
	{
		CSceneManager::GetInstance()->LoadScene(SceneList::Title);
	}

	// 敵のHPが０になったとき.
	if(m_pEnemy->GetCharaInfo().HP == 0)
	{
		CSceneManager::GetInstance()->LoadScene(SceneList::Title);
	}

	CollisionJudge();


#if _DEBUG
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
	m_pCylinder->Render( m_mView, m_mProj, m_Light );

	// エフェクトの描画.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );
}


//-----------------------------------------------------------------------------
//		当たり判定関数.
//-----------------------------------------------------------------------------
void CGame::CollisionJudge()
{
	MeshCollider PlayerEgg, Floor, Cylinder;

	Cylinder.SetVertex(
		m_pCylinder->GetPos(),
		m_pCylinder->GetRot(),
		m_pCylinder->GetScale(),
		m_pCylinder->GetVertices());

	// 地面データ設定.
	Floor.SetVertex(
		m_pGround->GetPos(),
		m_pGround->GetRot(),
		m_pGround->GetScale(),
		m_pFloor->GetVertices());

	// プレイヤーデータ設定.
	PlayerEgg.SetVertex(
		m_pPlayer->GetPos(),
		m_pPlayer->GetRot(),
		m_pPlayer->GetScale(),
		m_pEgg->GetVertices());


	// 卵と床の判定を返す.
	CollisionPoints pointspef = m_pGJK->GJK(PlayerEgg, Floor);
	// 卵と円柱の判定を返す.
	CollisionPoints pointspec = m_pGJK->GJK(Cylinder, PlayerEgg);


	//------------------------------------------------------------------------------
	//		卵と床の判定処理.
	//------------------------------------------------------------------------------
	if (pointspef.Col)
	{
		// プレイヤーにかかる重力をリセットする.
		m_pPlayer->ResetGravity();
		// ジャンプを可能にする.
		m_pPlayer->CanJump();

		// 法線が下方向を向いている場合（地面に衝突している）.
		if (pointspef.Normal.y < 0.f)
		{
			// 衝突深度に基づいてプレイヤーを押し戻す.
			D3DXVECTOR3 SetPos = m_pPlayer->GetPos() - pointspef.Normal * pointspef.Depth;

			// プレイヤーの位置を修正後の座標に更新.
			m_pPlayer->SetPos(SetPos);
		}
		// 法線が下方向以外（壁や斜面）を向いている場合.
		else {
			D3DXVec3Normalize(&pointspef.Normal, &pointspef.Normal);	// 正規化.

			// プレイヤーの現在の移動ベクトルを取得.
			D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();
			// プレイヤーの移動ベクトルと法線ベクトルの内積を計算（進行方向と法線の重なり具合を取得）.
			float Dot = D3DXVec3Dot(&PlayerMove, &pointspef.Normal);
			// 法線方向の移動成分を除去して、壁に沿った移動成分のみを残す.
			PlayerMove = PlayerMove - Dot * pointspef.Normal;
			// 修正した移動ベクトルをプレイヤーに加算.
			m_pPlayer->AddVec(PlayerMove);
		}
	}
	else {
		// プレイヤーにかかる重力を増やす.
		m_pPlayer->AddGravity();
		// ジャンプで加算される値を減らす.
		m_pPlayer->JumpPowerDec();
	}

	// プレイヤーに重力を加える.
	m_pPlayer->UseGravity();


	//------------------------------------------------------------------------------
	//		卵と柱の判定処理.
	//------------------------------------------------------------------------------
	if (pointspec.Col) {
		// 法線を正規化して単位ベクトルにする（計算精度を高めるため）.
		D3DXVec3Normalize(&pointspec.Normal, &pointspec.Normal);

		// プレイヤーの現在の移動ベクトルを取得.
		D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();
		// プレイヤーの移動ベクトルと法線ベクトルの内積を計算（進行方向と法線の重なり具合を取得）.
		float Dot = D3DXVec3Dot(&PlayerMove, &pointspec.Normal);
		// 法線方向の移動成分を除去して、壁に沿った移動成分のみを残す.
		PlayerMove = PlayerMove - Dot * pointspec.Normal;


		// 修正した移動ベクトルをプレイヤーに適用.
		m_pPlayer->AddVec(PlayerMove);
		m_pPlayer->SetPos(m_pPlayer->GetPos() + pointspec.Normal * pointspec.Depth);

		if (pointspec.Depth < 0.05f) {
			m_pPlayer->AddVec(-m_pPlayer->GetMoveVec());
		}
	}

	//------------------------------------------------------------------------------
	//		地面とカメラレイの判定(弾の到着地点に使用する).
	//------------------------------------------------------------------------------
	auto [hit, hitpos, length] = m_pGround->IsHitForRay(CCamera::GetInstance()->GetRay());
	if( hit ) { 
		CCamera::GetInstance()->SetRayHit(hitpos); 
	}
	else {
		//auto [hit, hitpos, length] = m_pCylinder->IsHitForRay(CCamera::GetInstance()->GetRay());
		CCamera::GetInstance()->SetRayHit(hitpos);
	}

	//------------------------------------------------------------------------------
	//		プレイヤーの当たり判定処理をする.
	//------------------------------------------------------------------------------
	m_pPlayer->Collision(m_pEnemy, Floor, Cylinder);




#if _DEBUG
	//-----------------------------------------------------
	// キャラクターウィンドウ.
	//-----------------------------------------------------

	ImGui::Begin("CharaWindow");

	ImGui::Text("%d", m_pPlayer->GetCharaInfo().HP);
	ImGui::Text("%d", m_pEnemy->GetCharaInfo().HP);

	ImGui::End();



	//-----------------------------------------------------
	// プレイヤーウィンドウ.
	//-----------------------------------------------------
	ImGui::Begin("PlayerWindow");

	D3DXVECTOR3 playerpos = m_pPlayer->GetPos();
	ImGui::Text("%f,%f,%f", playerpos.x, playerpos.y, playerpos.z);
	D3DXVECTOR3 playersumvec = m_pPlayer->GetMoveVec();
	ImGui::Text("%f,%f,%f", playersumvec.x, playersumvec.y, playersumvec.z);

	ImGui::End();

	//-----------------------------------------------------
	// 銃ウィンドウ.
	//-----------------------------------------------------
	ImGui::Begin("CameraWindow");

	D3DXVECTOR3 camrot = CCamera::GetInstance()->GetRot();
	ImGui::Text("%f,%f,%f", camrot.x, camrot.y, camrot.z);
	ImGui::End();
#endif
}