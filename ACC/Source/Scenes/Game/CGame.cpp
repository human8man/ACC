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

	, m_pEgg		( nullptr )
	, m_pFloor		( nullptr )
	, m_pCylinder	( nullptr )
	
	, m_pPlayer		( nullptr )
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
	m_pEgg		= new CStaticMesh();
	m_pFloor	= new CStaticMesh();
	m_pCylinder = new CStaticMesh();

	m_pPlayer	= new CPlayer();
	m_pGround	= new CGround();
	m_pCamRay	= new CRay();
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
	m_pPlayer->AttachMesh( *m_pEgg);
	m_pGround->AttachMesh( *m_pFloor);

	// キャラクターの初期座標を設定.
	m_pPlayer->SetPos( 0.f, 14.f, 6.f );
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
	SAFE_DELETE(m_pCamRay);
	SAFE_DELETE(m_pGround);
	SAFE_DELETE(m_pPlayer);
	SAFE_DELETE(m_pCylinder);
	SAFE_DELETE(m_pEgg);
	SAFE_DELETE(m_pFloor);

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
	//if(m_pEnemy->GetCharaInfo().HP == 0)
	//{
	//	CSceneManager::GetInstance()->LoadScene(SceneList::Title);
	//}

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
	m_pCylinder->Render( m_mView, m_mProj, m_Light );

	// エフェクトの描画.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );
}


//-----------------------------------------------------------------------------
//		当たり判定関数.
//-----------------------------------------------------------------------------
void CGame::CollisionJudge()
{
	//		プレイヤーとか敵クラスにいつか実装する内容ではあるが
	//		今はその時ではないので "内容" を忘れ "ないよう" にコメント化.
	
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
	//		m_pMeshEgg->GetVertices());

	//	CollisionPoints points = m_pGJK->GJK(m_MeshA, m_MeshB);

	//	// あたった場合削除.
	//	if (points.HasCollision)
	//	{
	//		SAFE_DELETE(m_pBullets[i]);
	//		m_pBullets.erase(m_pBullets.begin() + i);
	//		--i;
	//	}
	//}
	MeshCollider Egg,Floor,Cylinder;

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
	Egg.SetVertex(
		m_pPlayer->GetPos(),
		m_pPlayer->GetRot(),
		m_pPlayer->GetScale(),
		m_pEgg->GetVertices());

	// プレイヤーに重力を加える.
	m_pPlayer->UseGravity();

	// 卵と床の判定を返す.
	CollisionPoints pointsef = m_pGJK->GJK(Egg,Floor);
	// 卵と円柱の判定を返す.
	CollisionPoints pointsec = m_pGJK->GJK(Cylinder, Egg);

	if (pointsef.Col)
	{
		if (pointsef.Normal.y < 0.f) // 法線が下方向を向いている場合（地面に衝突している）.
		{
			// 衝突深度に基づいてプレイヤーを押し戻す.
			D3DXVECTOR3 SetPos = m_pPlayer->GetPos() - pointsef.Normal * pointsef.Depth;

			// プレイヤーの位置を修正後の座標に更新.
			m_pPlayer->SetPos(SetPos);
		}
		else // 法線が下方向以外（壁や斜面）を向いている場合.
		{
			// 法線を正規化して単位ベクトルにする（計算精度を高めるため）.
			D3DXVec3Normalize(&pointsef.Normal, &pointsef.Normal);

			// プレイヤーの現在の移動ベクトルを取得.
			D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();

			// プレイヤーの移動ベクトルと法線ベクトルの内積を計算（進行方向と法線の重なり具合を取得）.
			float Dot = D3DXVec3Dot(&PlayerMove, &pointsef.Normal);

			// 法線方向の移動成分を除去して、壁に沿った移動成分のみを残す.
			PlayerMove = PlayerMove - Dot * pointsef.Normal;

			// 修正した移動ベクトルをプレイヤーに適用.
			m_pPlayer->AddVec(PlayerMove);
		}
	}


	if (pointsec.Col) {
		// 法線を正規化して単位ベクトルにする（計算精度を高めるため）.
		D3DXVec3Normalize(&pointsec.Normal, &pointsec.Normal);

		// プレイヤーの現在の移動ベクトルを取得.
		D3DXVECTOR3 PlayerMove = m_pPlayer->GetMoveVec();

		// プレイヤーの移動ベクトルと法線ベクトルの内積を計算（進行方向と法線の重なり具合を取得）.
		float Dot = D3DXVec3Dot(&PlayerMove, &pointsec.Normal);

		// 法線方向の移動成分を除去して、壁に沿った移動成分のみを残す.
		PlayerMove = PlayerMove - Dot * pointsec.Normal;

		// 修正した移動ベクトルをプレイヤーに適用.
		m_pPlayer->AddVec(PlayerMove);
		m_pPlayer->SetPos(m_pPlayer->GetPos() + pointsec.Normal * pointsec.Depth);

		if (pointsec.Depth < 0.05f){
			m_pPlayer->AddVec(-m_pPlayer->GetMoveVec());
		}
	}
 
	// 地面とカメラレイの判定(弾の到着地点に使用する).
	auto [hit, hitpos, length] = m_pGround->IsHitForRay(CCamera::GetInstance()->GetRay());
	
#if _DEBUG
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

	//-----------------------------------------------------
	// 当たり判定のウィンドウ.
	//-----------------------------------------------------
	ImGui::Begin("ColWindow");

	//if(hit) { ImGui::Text("true");	}
	//else	{ ImGui::Text("false");	}
	//ImGui::Text("%f,%f,%f",hitpos.x,hitpos.y,hitpos.z );
	//ImGui::Text("%f",length );

	if(pointsec.Col) { ImGui::Text("true");	}
	else			 { ImGui::Text("false");}
	ImGui::Text("%f,%f,%f", pointsec.Normal.x, pointsec.Normal.y, pointsec.Normal.z);
	ImGui::Text("%f", pointsec.Depth);

	ImGui::End();
#endif
}