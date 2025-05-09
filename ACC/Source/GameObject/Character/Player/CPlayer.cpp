#include "CPlayer.h"
#include "Effect/CEffect.h"
#include "Camera/CCamera.h"
#include "DirectX/CDirectX11.h"
#include "Character/Enemy/CEnemy.h"
#include "FileManager/FileManager.h"
#include "DirectInput/CDirectInput.h"
#include "DirectSound/CSoundManager.h"

namespace {
	// キャラクターCSVのパス.
	constexpr char CharaCSVPath[] = "Data\\CSV\\CharaStatus.csv";
}


//============================================================================
//		プレイヤークラス.
//============================================================================
CPlayer::CPlayer()
	: m_pGJK		( nullptr )
	, m_MoveSpeed	( 0.2f )
	, m_CamRevision	( 4.f )
	, m_SumVec		( ZEROVEC3 )
	, m_AutoAim		( false )
	, m_Homing		( false )
	, m_WallHack	( false )
{
	// 初期化.
	m_CharaInfo.HP = m_CharaInfo.MaxHP;
	m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;

	// キャラクターCSVの情報保存用.
	std::unordered_map<std::string, std::string> m_StateList;
	// キャラクターCSVの情報取得.
	m_StateList = FileManager::CSVLoad(CharaCSVPath);

	// 空でない場合は、外部で調整するべき変数の値を入れていく.
	if (!m_StateList.empty())
	{
		m_MoveSpeed		= StrToFloat(m_StateList["PlayerMoveSpeed"]);
		m_CamRevision	= StrToFloat(m_StateList["CameraRevision"]);
	}
}
CPlayer::~CPlayer()
{
}


//============================================================================
//		更新処理.
//============================================================================
void CPlayer::Update(std::unique_ptr<CEnemy>& chara)
{
	// 毎フレームリセットする.
	m_SumVec = ZEROVEC3;
	m_Hit = false;

	// カメラに向きを合わせる.
	m_vRotation.y = CCamera::GetInstance()->GetRot().y;
	
	// 0以上のものがある場合カウントをする.
	if ( m_DashTime >= 0.f )	{ m_DashTime -= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_ReloadTime >= 0.f )	{
		m_ReloadTime -= CTime::GetInstance()->GetDeltaTime(); 
		// リロード終了時にSEを鳴らす.
		if (m_ReloadTime < 0.f) {
			CSoundManager::GetInstance()->Play(CSoundManager::enList::SE_ReloadEnd);
		}
	}
	if ( m_DashCoolTime >= 0.f )	{ m_DashCoolTime	-= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_BulletCoolTime >= 0.f )	{ m_BulletCoolTime	-= CTime::GetInstance()->GetDeltaTime(); }

	// 入力処理.
	KeyInput(chara);

	// 入力処理後にカメラ座標をセット.
	if (!CCamera::GetInstance()->GetMoveCamera()) {
		// プレイヤー位置 + プレイヤーの高さを含んだ座標を渡す.
		D3DXVECTOR3 campos = m_vPosition;
		campos.y += m_CamRevision;
		CCamera::GetInstance()->SetPosition(campos);
	}

	// ホーミングのチートが有効な場合.
	if (m_Homing) {
		for (size_t i = 0; i < m_pBullets.size(); ++i) 
		{
			D3DXVECTOR3 shootdir, EnemyPos;

			// 敵座標を取得(原点だと地面に当たるため少し浮かす).
			EnemyPos = chara->GetPos();
			EnemyPos.y += 0.5f;

			// 弾座標から敵座標のベクトルを出す.
			shootdir = EnemyPos - m_pBullets[i]->GetPos();
			D3DXVec3Normalize(&shootdir, &shootdir); // 正規化.

			// 弾の初期位置,移動方向の単位ベクトル,速度を設定.
			m_pBullets[i]->Init( m_pBullets[i]->GetPos(), shootdir, m_BulletSpeed);
		}
	}

	// キャラクターの更新処理.
	CCharacter::Update();
}


//============================================================================
//		描画処理.
//============================================================================
void CPlayer::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	// プレイヤーは描画しない.
	// CCharacter::Draw( View, Proj, Light );

	// エフェクト事に必要なハンドルを用意.
	static ::EsHandle hEffect = -1;

	D3DXVECTOR4 color = { 0.f,0.f,1.f,1.f };

	// 弾とそのエフェクトの描画.
	for (size_t i = 0; i < m_pBullets.size(); ++i) { 
		hEffect = CEffect::Play(CEffect::BulletSmoke, m_pBullets[i]->GetPos());

		// ウォールハック中はメッシュ線が出現.
		if (m_WallHack) {
			CDirectX11::GetInstance()->SetDepth(false);
			m_pMeshLine->DrawMeshWireframeFromVertices(*m_pBullets[i]->GetMesh(), *m_pBullets[i], View, Proj, color, 5.f);
			m_pMeshLine->Render(View, Proj);
			CDirectX11::GetInstance()->SetDepth(true);
		}
	}

	// 銃の描画.
	m_pGun->Draw(View, Proj, Light);
}


//============================================================================
//		当たり判定処理.
//============================================================================
void CPlayer::Collision(std::unique_ptr<CEnemy>& egg, Collider floor, Collider cylinder)
{
	Collider Bullet,enemyegg;

	// エフェクト事に必要なハンドルを用意.
	static ::EsHandle hEffect = -1;

	// 敵データを取得.
	enemyegg.SetVertex( egg->GetObjeInfo(), egg->GetMesh()->GetVertices());

	// 弾の判定.
	for (size_t i = 0; i < m_pBullets.size(); ++i) {

		// 弾データを取得.
		Bullet.SetVertex( m_pBullets[i]->GetObjeInfo(), m_pMeshBullet->GetVertices());

		// 当たり判定情報用の変数を宣言.
		CollisionPoints pointsbc, pointsbf, pointsbe;
		pointsbc = m_pGJK->GJK(Bullet, cylinder);
		pointsbf = m_pGJK->GJK(Bullet, floor);
		pointsbe = m_pGJK->GJK(Bullet, enemyegg);

		// 柱や床にあたった場合削除.
		if (pointsbc.Col || pointsbf.Col)
		{
			// エフェクトの再生.
			hEffect = CEffect::Play(CEffect::Dust, m_pBullets[i]->GetPos() - m_pBullets[i]->GetMoveVec() * 2);

			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}

		// エネミーと弾が当たった場合.
		if ( pointsbe.Col ) {
			// ヘッドショット判定(気室判定).
			if (m_pBullets[i]->GetPos().y < egg->GetPos().y + m_EggAirRoomY) {
				// HPをクリティカルダメージ分減らす.
				egg->CritDamage();
				// エフェクトの再生.
				hEffect = CEffect::Play(CEffect::CritHit, egg->GetPos());
				// 命中種類の設定.
				m_HitKind = HitKind::Crit;
				// クリティカル命中音を鳴らす.
				CSoundManager::GetInstance()->Play(CSoundManager::enList::SE_CritHit);
			}
			else {
				// HPを胴体ダメージ分減らす.
				egg->BodyDamage();
				// エフェクトがズレていたのでずらしてからエフェクトの再生.
				D3DXVECTOR3 enemypos = egg->GetPos();
				enemypos.y += 2.f;
				hEffect = CEffect::Play(CEffect::Hit, enemypos);
				// 命中種類の設定.
				m_HitKind = HitKind::Hit;
				// 命中音を鳴らす.
				CSoundManager::GetInstance()->Play(CSoundManager::enList::SE_Hit);
			}

			// 命中した.
			m_Hit = true;

			// 当たったあとは削除.
			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}
	}
}


//-----------------------------------------------------------------------------
//		キー入力処理.
//-----------------------------------------------------------------------------
void CPlayer::KeyInput(std::unique_ptr<CEnemy>& chara)
{
	// エフェクト事に必要なハンドルを用意.
	static ::EsHandle hEffect = -1;

	CKey* Key = CInput::GetInstance()->CDKeyboard();
	CMouse* Mouse = CInput::GetInstance()->CDMouse();

	//----------------------------------------------------------------------
	//		WASDで移動.
	//----------------------------------------------------------------------

	// ダッシュ中は操作できないようにする.
	if (m_DashTime <= 0.f) {
		// 操作が可能な間は初期化する.
		DashVec = ZEROVEC3;

		D3DXVECTOR3 camDir;
		// カメラの向きベクトルを取得.
		if (m_AutoAim) {
			camDir = chara->GetPos() - m_vPosition;
			camDir.y = 0.f;	// Y情報があると飛び始めるのでYの要素を抜く.
			D3DXVec3Normalize(&camDir, &camDir); // 正規化.
		}
		else {
			camDir = CCamera::GetInstance()->GetCamDir();
			camDir.y = 0.f;	// Y情報があると飛び始めるのでYの要素を抜く.
			D3DXVec3Normalize(&camDir, &camDir); // 正規化.
		}

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
	if (m_DashCoolTime <= 0.f) { m_CanDash = true; }

	// SHIFTでダッシュ.
	if (Key->IsKeyAction(DIK_LSHIFT) && m_CanDash) {
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

			// カメラ方向 × 移動速度 × ダッシュ倍率のベクトルを出す.
			DashVec = camDir * m_MoveSpeed * m_DashSpeed;
			m_CanDash = false;
		}
		else {
			// 合計ベクトルにダッシュ倍率を変えた値を出す.
			DashVec = m_SumVec * m_DashSpeed;
			m_CanDash = false;
		}
		CSoundManager::GetInstance()->Play(CSoundManager::enList::SE_Dash);
	}

	//----------------------------------------------------------------------
	//		ジャンプ処理.
	//----------------------------------------------------------------------
	if (Key->IsKeyAction(DIK_SPACE) && m_CanJump) {
		m_JumpPower = m_JumpPowerMax;
		m_CanJump = false;
		CSoundManager::GetInstance()->Play(CSoundManager::enList::SE_Jump);
	}
	// ジャンプ力をY値に加算.
	m_vPosition.y += m_JumpPower;


	// カメラのレイHit座標から発射地点のベクトルを計算.
	D3DXVECTOR3 shootdir = CCamera::GetInstance()->GetRayHit() - m_pGun->GetShootPos();
	D3DXVec3Normalize(&shootdir, &shootdir);	// 正規化.

	//----------------------------------------------------------------------
	//		左クリックで射撃.
	//----------------------------------------------------------------------

	// クールタイムが終了していたら射撃可能.
	if (m_BulletCoolTime <= 0.f) { m_CanShot = true; }

	// 左クリックが押された場合.
	if (Mouse->IsLAction()){
		// 射撃条件が整っている場合.
		if (m_CanShot && m_CharaInfo.Ammo != 0 && m_ReloadTime <= 0 || m_TriggerHappy) 
		{
			// 連射モードでない場合.
			if (!m_TriggerHappy) {
				// クールタイムや残弾数の設定.
				m_CanShot = false;
				m_CharaInfo.Ammo--;
				m_BulletCoolTime = m_BulletCoolTimeMax;
			}

			// 弾を作成する.
			m_pBullets.push_back(std::make_unique<CBullet>());

			m_pBullets.back()->AttachMesh(*m_pMeshBullet);	// メッシュを設定.
			m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);	// nullにならないように見えない座標に初期設定.
			m_pBullets.back()->SetScale(5.f, 5.f, 5.f);		// サイズを設定.

			// 弾の初期位置,移動方向の単位ベクトル,速度を設定.
			m_pBullets.back()->Init(
				m_pGun->GetShootPos(),
				shootdir,
				m_BulletSpeed);
			
			hEffect = CEffect::Play(CEffect::GunFire, m_pGun->GetShootPos());

			// 射撃音を鳴らす.
			CSoundManager::GetInstance()->Play(CSoundManager::enList::SE_Shot);
		}
		else if (m_CanShot && m_CharaInfo.Ammo == 0 && m_ReloadTime <= 0) {
			// 空打ち音を鳴らす.
			CSoundManager::GetInstance()->Play(CSoundManager::enList::SE_NoAmmo);
		}
	}

	//----------------------------------------------------------------------
	//		Rでリロード.
	//----------------------------------------------------------------------
	if (Key->IsKeyAction(DIK_R)) {
		m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;
		m_ReloadTime = m_ReloadTimeMax;
		CSoundManager::GetInstance()->Play(CSoundManager::enList::SE_Reload);
	}

	//----------------------------------------------------------------------
	//		チート関連.
	//----------------------------------------------------------------------
	if (Key->IsKeyAction(DIK_1)) { m_AutoAim = !m_AutoAim; }
	if (Key->IsKeyAction(DIK_2)) { m_Homing = !m_Homing; }
	if (Key->IsKeyAction(DIK_3)) { m_WallHack = !m_WallHack; }
	if (Key->IsKeyAction(DIK_4)) { m_TriggerHappy = !m_TriggerHappy; RAINBOW_WINDOW = m_TriggerHappy; }


	// 合計のベクトル量分位置を更新.
	m_vPosition += m_SumVec + DashVec;
}