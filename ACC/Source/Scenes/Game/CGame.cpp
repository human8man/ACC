#include "CGame.h"

#include "DirectSound/CSoundManager.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectInput/CDirectInput.h"

#include "Effect/CEffect.h"

#include "Camera/CCamera.h"
#include "Object/Ground/CGround.h"
#include "Character/Player/CPlayer.h"
#include "Object/Bullet/CBullet.h"
#include "Object/Gun/CGun.h"

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
	, m_pGun			( nullptr )
	, m_pBullets		()

	, m_pGJK			( nullptr )

	, m_pCamRay			( nullptr )

	, m_PLayerSize		{ 0.f, 2.f, 0.f, }
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
	m_pMeshBullet	= new CStaticMesh();
	m_pMeshGun		= new CStaticMesh();
	m_pPlayer		= new CPlayer();
	m_pGun			= new CGun();

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
	m_pMeshGun		->Init( _T("Data\\Mesh\\Static\\Gun\\Gun.x" ) );
	m_pMeshFighter	->Init( _T("Data\\Mesh\\Static\\Player\\egg.x" ) );
	m_pMeshGround	->Init( _T("Data\\Mesh\\Static\\Stage\\stage.x" ) );
	m_pMeshBullet	->Init( _T("Data\\Mesh\\Static\\Bullet\\bullet.x" ) );

	m_pPlayer->AttachMesh( *m_pMeshFighter );
	m_pGround->AttachMesh( *m_pMeshGround );
	m_pGun	 ->AttachMesh( *m_pMeshGun );

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
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		SAFE_DELETE(m_pBullets[i]);
	}
	
	SAFE_DELETE(m_pCamRay);

	SAFE_DELETE(m_pGround);

	SAFE_DELETE(m_pGun);
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


	CKey*	Key	  = CDInput::GetInstance()->CDKeyboard();
	CMouse* Mouse = CDInput::GetInstance()->CDMouse();

	// プレイヤーの更新.
	m_pPlayer->Update();

	// プレイヤーの更新後にプレイヤー座標にカメラをセット.
	if (!CCamera::GetInstance()->GetMoveCamera()) {
		 CCamera::GetInstance()->SetPosition(m_pPlayer->GetPos() + m_PLayerSize);
	}

	// カメラの更新.
	CCamera::GetInstance()->Update();
	D3DXVECTOR3 direction = CCamera::GetInstance()->GetCamDir();


	// クォータニオンで銃を回す処理.
	D3DXVECTOR3 Bpos = m_pGun->GetPos();
	float radius = 1.f;	// 半径.
	m_Angle += 1.f;		// 回転角度（フレームごとに増加）.

	UpdateGunPosition(
		m_pPlayer->GetPos(),
		Bpos,
		radius,
		D3DXToRadian(CCamera::GetInstance()->GetRot().y));

	m_pGun->SetPos(Bpos);
	// Yaw（Y軸回転）の計算.
	float yaw = atan2f(direction.z, direction.x);
	m_pGun->SetRot( 0.f, -D3DXToRadian(CCamera::GetInstance()->GetRot().y)-1.5f, 0.f );

	// カメラ側のキー操作を無効にする.
	if (Key->IsKeyAction(DIK_F2)) { CCamera::GetInstance()->ChangeCanMove(); }
	if (Key->IsKeyAction(DIK_F3)) { CCamera::GetInstance()->ChangeUseMouse(); }
	
	// 左クリック.
	if (Mouse->IsLAction()) {
		m_pBullets.push_back(new CBullet()); // m_pBullets に追加

		m_pBullets.back()->AttachMesh(*m_pMeshBullet);
		m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);

		// ベクトルのノーマライズ（方向のみを抽出）.
		D3DXVec3Normalize(&direction, &direction);

		// Pitch（X軸回転）の計算.
		float pitch = asinf(direction.y); // Y方向から計算.

		// ラジアンから角度へ変換.
		yaw		= D3DXToDegree(yaw);
		pitch	= D3DXToDegree(pitch);

		D3DXVECTOR3 rot( pitch, yaw, 0.f );

		// 初期位置,移動方向の単位ベクトル,弾の向き,速度がいるため保留.
		m_pBullets.back()->Init(
			m_pGun->GetShootPos(),
			CCamera::GetInstance()->GetCamDir(),
			direction,
			0.01f );
	}



	// 弾の更新.
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		// 各弾を更新
		m_pBullets[i]->Update();

		// 一定時間経過したら削除.
		if (m_pBullets[i]->DeleteBullet()) {
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}
	}
}


//============================================================================
//		描画処理.
//============================================================================
void CGame::Draw()
{
	CCamera::GetInstance()->Camera(m_mView);
	CSceneBase::Projection(m_mProj);

	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		m_pBullets[i]->Draw(m_mView, m_mProj, m_Light);
	}

	m_pGround->Draw( m_mView, m_mProj, m_Light );
	//m_pPlayer->Draw( m_mView, m_mProj, m_Light );
	m_pGun->Draw(m_mView, m_mProj, m_Light);

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
	// 弾とプレイヤーの判定.
	for (size_t i = 0; i < m_pBullets.size(); ++i) {

		m_MeshB.SetVertex(
			m_pBullets[i]->GetPos(),
			m_pBullets[i]->GetRot(),
			m_pBullets[i]->GetScale(),
			m_pMeshBullet->GetVertices());

		m_MeshA.SetVertex(
			m_pPlayer->GetPos(),
			m_pPlayer->GetRot(),
			m_pPlayer->GetScale(),
			m_pMeshFighter->GetVertices());

		CollisionPoints points = m_pGJK->GJK(m_MeshA, m_MeshB);

		// あたった場合削除.
		if (points.HasCollision)
		{
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}
	}

	auto [hit, hitpos, length] = m_pGround->IsHitForRay(CCamera::GetInstance()->GetRay());
	
#if _DEBUG
	// 銃ウィンドウ.
	ImGui::Begin("CameraWindow");

	D3DXVECTOR3 camrot = CCamera::GetInstance()->GetRot();
	ImGui::Text("%f,%f,%f", camrot.x, camrot.y, camrot.z);
	ImGui::DragFloat3("##Position", camrot, 0.1f);

	ImGui::End();


	// 銃ウィンドウ.
	ImGui::Begin("BulletWindow");

	D3DXVECTOR3 shootpos = m_pGun->GetPos();
	ImGui::Text("%f,%f,%f", shootpos.x, shootpos.y, shootpos.z);
	ImGui::DragFloat3("##Position", shootpos, 0.1f);
	m_pGun->SetPos(shootpos);

	ImGui::End();


	// 弾のウィンドウ.
	ImGui::Begin("BulletWindow");

	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		D3DXVECTOR3 rot = m_pBullets[i]->GetRot();
		D3DXVECTOR3 pos = m_pBullets[i]->GetPos();
		ImGui::Text("------------------------------------");
		ImGui::DragFloat3("rot", rot, 0.01f);
		ImGui::DragFloat3("pos", pos, 0.01f);
	}
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


//-----------------------------------------------------------------------------
//		クォターニオンお試し.
//-----------------------------------------------------------------------------
void CGame::UpdateSatellitePosition(
	const D3DXVECTOR3& center, 
	D3DXVECTOR3& pos, 
	float radius, 
	float angle)
{
	// 回転軸（今回はY軸回り）.
	D3DXVECTOR3 rotationAxis(0.f, 1.f, 0.f);

	// 初期位置（BがAからradius離れた位置）.
	D3DXVECTOR3 initialPosition( radius, 0.f, 0.f );

	// クォータニオンを使って回転を計算.
	D3DXQUATERNION rotationQuat;
	D3DXQuaternionRotationAxis(&rotationQuat, &rotationAxis, D3DXToRadian(angle));

	D3DXVECTOR3 rotatedPosition; // 回転後の座標を計算.
	D3DXMATRIX rotationMatrix;	 // 回転行列を事前に宣言.
	D3DXMatrixRotationQuaternion(&rotationMatrix, &rotationQuat);
	D3DXVec3TransformCoord(&rotatedPosition, &initialPosition, &rotationMatrix);

	// Aを基準に座標を加算.
	pos = center + rotatedPosition;
}

void CGame::UpdateGunPosition(
	const D3DXVECTOR3& center, 
	D3DXVECTOR3& pos, 
	float radius,
	float playerYaw)
{
	// 初期位置（プレイヤーの右方向にradius分だけ離れた位置）.
	D3DXVECTOR3 initialPosition(radius, 0.f, 0.f);
	D3DXVECTOR3 Upvec(0.f, -1.f, 0.f);

	// プレイヤーのyaw回転を表すクォータニオン.
	D3DXQUATERNION playerYawQuat;
	D3DXQuaternionRotationAxis(&playerYawQuat, &Upvec, playerYaw);

	// クォータニオンを回転行列に変換.
	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationQuaternion(&rotationMatrix, &playerYawQuat);

	// 初期位置を回転して最終的な位置を計算.
	D3DXVECTOR3 rotatedPosition;
	D3DXVec3TransformCoord(&rotatedPosition, &initialPosition, &rotationMatrix);

	// プレイヤーの位置を基準に最終位置を計算.
	pos = center + rotatedPosition;
}