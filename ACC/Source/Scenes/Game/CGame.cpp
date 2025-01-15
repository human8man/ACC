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

	, m_pMeshEgg	( nullptr )
	, m_pMeshGround	( nullptr )
	
	, m_pPlayer		( nullptr )
	, m_pGround		( nullptr )

	, m_pGJK		( nullptr )
	, m_pCamRay		( nullptr )

	, m_MeshA		()
	, m_MeshB		()

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
	m_pMeshEgg		= new CStaticMesh();
	m_pMeshGround	= new CStaticMesh();
	m_pPlayer		= new CPlayer();
	m_pGround		= new CGround();
	m_pCamRay		= new CRay();
}

//============================================================================
//		ロードデータ関数.
//============================================================================
HRESULT CGame::LoadData()
{
	// スタティックメッシュの読み込み.
	m_pMeshEgg	 ->Init( _T("Data\\Mesh\\Static\\Player\\egg.x" ) );
	m_pMeshGround->Init( _T("Data\\Mesh\\Static\\Stage\\stage.x" ) );

	// メッシュをアタッチする.
	m_pPlayer->AttachMesh( *m_pMeshEgg );
	m_pGround->AttachMesh( *m_pMeshGround );

	// キャラクターの初期座標を設定.
	m_pPlayer->SetPos( 0.f, 0.f, 6.f  );

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
	SAFE_DELETE(m_pMeshGround);
	SAFE_DELETE(m_pMeshEgg);

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

	// エフェクトの描画.
	CEffect::GetInstance()->Draw( m_mView, m_mProj, m_Light );
	CollisionJudge();
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
	
	// 地面データ設定.
	m_MeshB.SetVertex(
		m_pGround->GetPos(),
		m_pGround->GetRot(),
		m_pGround->GetScale(),
		m_pMeshGround->GetVertices());

	// プレイヤーデータ設定.
	m_MeshA.SetVertex(
		m_pPlayer->GetPos(),
		m_pPlayer->GetRot(),
		m_pPlayer->GetScale(),
		m_pMeshEgg->GetVertices());
	
	CollisionPoints points = m_pGJK->GJK(m_MeshA, m_MeshB);

	// 衝突している場合.
	if (points.HasCollision) {
		// プレイヤーの元座標から深度分だけ戻す.
		m_pPlayer->SetPos(points.Normal * points.Depth);
	}
	
 
	// 地面とカメラレイの判定(弾の到着地点に使用する).
	auto [hit, hitpos, length] = m_pGround->IsHitForRay(CCamera::GetInstance()->GetRay());
	
#if _DEBUG
	// 銃ウィンドウ.
	ImGui::Begin("CameraWindow");

	D3DXVECTOR3 camrot = CCamera::GetInstance()->GetRot();
	ImGui::Text("%f,%f,%f", camrot.x, camrot.y, camrot.z);
	ImGui::DragFloat3("##Position", camrot, 0.1f);

	ImGui::End();

	// レイの当たり判定のウィンドウ.
	ImGui::Begin("ColWindow");

	if(hit) { ImGui::Text("true");	}
	else	{ ImGui::Text("false");	}
	ImGui::Text("%f,%f,%f",hitpos.x,hitpos.y,hitpos.z );
	ImGui::Text("%f",length );

	ImGui::End();
#endif
}