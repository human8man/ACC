#include "CPlayer.h"
#include "DirectSound/CSoundManager.h"
#include "Common/DirectInput/CDirectInput.h"
#include "Camera/CCamera.h"
#include "Character/Enemy/CEnemy.h"

#if _DEBUG
#include "ImGui/CImGui.h"
#endif

//============================================================================
//		プレイヤークラス.
//============================================================================
CPlayer::CPlayer()
	: m_pGJK		( nullptr )
	, m_TurnSpeed	( 0.1f )
	, m_MoveSpeed	( 0.2f )
	, m_CamRevision	( 2.f )
	, m_SumVec		( ZEROVEC3 )
{
	m_CharaInfo.HP = 8;
}

CPlayer::~CPlayer()
{
}


//============================================================================
//		更新処理.
//============================================================================
void CPlayer::Update()
{
	// 毎フレームリセットする.
	m_SumVec = ZEROVEC3;

	// カメラに向きを合わせる.
	m_vRotation.y = CCamera::GetInstance()->GetRot().y;
	
	// ダッシュ関連で0以上のものがある場合カウントをする.
	if ( m_DashCoolTime >= 0.f ) { m_DashCoolTime -= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_DashTime >= 0.f ){ m_DashTime -= CTime::GetInstance()->GetDeltaTime(); }

	// 入力処理.
	KeyInput();

	// 入力処理後にカメラ座標をセット.
	if (!CCamera::GetInstance()->GetMoveCamera()) {
		// プレイヤー位置 + プレイヤーの高さを含んだ座標を渡す.
		D3DXVECTOR3 campos = m_vPosition;
		campos.y += m_CamRevision;
		 CCamera::GetInstance()->SetPosition(campos);
	}

	CCharacter::Update();
}


//============================================================================
//		描画処理.
//============================================================================
void CPlayer::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	// プレイヤーは描画しない.
	// CCharacter::Draw( View, Proj, Light );
	
	// 弾の描画.
	for (size_t i = 0; i < m_pBullets.size(); ++i) { m_pBullets[i]->Draw(View, Proj, Light); }

	// 銃の描画.
	m_pGun->Draw(View, Proj, Light);
}


//============================================================================
//		当たり判定処理.
//============================================================================
void CPlayer::Collision(std::unique_ptr<CEnemy>& egg, MeshCollider floor, MeshCollider cylinder)
{
	MeshCollider Bullet,enemyegg;
	
	// 敵データを取得.
	enemyegg.SetVertex(
		egg->GetPos(),
		egg->GetRot(),
		egg->GetScale(),
		egg->GetMesh()->GetVertices());

	// 弾の判定.
	for (size_t i = 0; i < m_pBullets.size(); ++i) {

		// 弾データを取得.
		Bullet.SetVertex(
			m_pBullets[i]->GetPos(),
			m_pBullets[i]->GetRot(),
			m_pBullets[i]->GetScale(),
			m_pMeshBullet->GetVertices());


		// 当たり判定情報用の変数を宣言.
		CollisionPoints pointsbc, pointsbf, pointsbe;
		pointsbc = m_pGJK->GJK(Bullet, cylinder);
		pointsbf = m_pGJK->GJK(Bullet, floor);
		pointsbe = m_pGJK->GJK(Bullet, enemyegg);


		// 柱や床にあたった場合削除.
		if (pointsbc.Col || pointsbf.Col) {
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}

		// エネミーと弾が当たった場合.
		if ( pointsbe.Col ) {
			// ヘッドショット判定(気室判定).
			if (m_pBullets[i]->GetPos().y < egg->GetPos().y + m_EggAirRoomY) { egg->DubleDecreHP(); }
			else  { egg->DecreHP(); }

			// 当たったあとは削除.
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}
	}
}


//-----------------------------------------------------------------------------
//		キー入力処理.
//-----------------------------------------------------------------------------
void CPlayer::KeyInput()
{
	CKey*	Key		= CDInput::GetInstance()->CDKeyboard();
	CMouse* Mouse	= CDInput::GetInstance()->CDMouse();
	

	//----------------------------------------------------------------------
	//		WASDで移動.
	//----------------------------------------------------------------------
	// ダッシュ中は操作できないようにする.
	if ( m_DashTime <= 0.f ) {
		// 操作が可能な間は初期化する.
		DashVec = ZEROVEC3;

		// カメラの向きベクトルを取得.
		D3DXVECTOR3 camDir = CCamera::GetInstance()->GetCamDir();
		camDir.y = 0.f;	// Y情報があると飛び始めるのでYの要素を抜く.
		D3DXVec3Normalize(&camDir, &camDir); // 正規化.

		// 移動する方向ベクトル.
		D3DXVECTOR3 forward(ZEROVEC3);
		D3DXVECTOR3 left(ZEROVEC3);
		D3DXVECTOR3 upvec(0, 1, 0);

		// 左ベクトルを求める.
		D3DXVec3Cross(&left, &camDir, &upvec);
		D3DXVec3Normalize(&left, &left);

		if (Key->IsKeyDown(DIK_W)) { forward += camDir; }
		if (Key->IsKeyDown(DIK_S)) { forward -= camDir; }
		if (Key->IsKeyDown(DIK_A)) { forward += left; }
		if (Key->IsKeyDown(DIK_D)) { forward -= left; }

		// 最終的なベクトル量を速度にかけ合計ベクトルに渡す.
		m_SumVec += forward * m_MoveSpeed;
	}

	//----------------------------------------------------------------------
	//		SHIFTでダッシュ.
	//----------------------------------------------------------------------
	// クールタイムが終了していたらダッシュ可能に.
	if ( m_DashCoolTime <= 0.f ) { m_CanDash = true; }

	// SHIFTでダッシュ.
	if ( Key->IsKeyAction(DIK_LSHIFT) && m_CanDash) {
		// ダッシュ時間の設定.
		m_DashTime = m_DashTimeMax;
		// ダッシュクールタイムの設定.
		m_DashCoolTime = m_DashCoolTimeMax;

		// 合計ベクトルに情報がない場合.
		if (m_SumVec == ZEROVEC3) {
			// カメラの向きベクトルを取得.
			D3DXVECTOR3 camDir = CCamera::GetInstance()->GetCamDir();
			camDir.y = 0.f;	// Y情報があると飛び始めるのでYの要素を抜く.
			D3DXVec3Normalize(&camDir, &camDir); // 正規化.
			DashVec = camDir * m_MoveSpeed * m_DashSpeed;
			m_CanDash = false;
		}
		else {
			DashVec = m_SumVec * m_DashSpeed;
			m_CanDash = false;
		}
	}

	//----------------------------------------------------------------------
	//		ジャンプ処理.
	//----------------------------------------------------------------------
	if ( Key->IsKeyAction(DIK_SPACE) && m_CanJump ) {
		m_JumpPower = m_JumpPowerMax; 
		m_CanJump = false; 
	}
	// ジャンプ力をY値に加算.
	m_vPosition.y += m_JumpPower;


	// カメラのレイHit座標から発射地点のベクトルを計算.
	D3DXVECTOR3 shootdir = CCamera::GetInstance()->GetRayHit() - m_pGun->GetShootPos();
	D3DXVec3Normalize(&shootdir, &shootdir);	// 正規化.

	//----------------------------------------------------------------------
	//		左クリックで射撃.
	//----------------------------------------------------------------------
	if (Mouse->IsLAction()) {
		m_pBullets.push_back(new CBullet());

		m_pBullets.back()->AttachMesh(*m_pMeshBullet);	// メッシュを設定.
		m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);	// nullにならないように見えない座標に初期設定.
		m_pBullets.back()->SetScale(5.f, 5.f, 5.f);	// サイズを設定.


		// 弾の初期位置,移動方向の単位ベクトル,速度を設定.
		m_pBullets.back()->Init(
			m_pGun->GetShootPos(),
			shootdir,
			0.8f );
	}

	// 合計のベクトル量分位置を更新.
	m_vPosition += m_SumVec + DashVec;

#if _DEBUG
	ImGui::Begin("DashWindow");
	ImGui::Text("DashCoolTime%f", m_DashCoolTime);
	ImGui::Text("DashTime%f", m_DashTime);
	ImGui::Text("DeltaTime%f", CTime::GetInstance()->GetDeltaTime() );
	ImGui::Text("%f,%f,%f", DashVec.x, DashVec.y, DashVec.z);
	ImGui::End();

#endif
}