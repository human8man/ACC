#include "Player.h"
#include "Effect/Effect.h"
#include "Camera/Camera.h"
#include "DirectX/DirectX11.h"
#include "Character/Enemy/Enemy.h"
#include "FileManager/FileManager.h"
#include "DirectInput/DirectInput.h"
#include "DirectSound/SoundManager.h"
#include "ImGui/CImGui.h"

namespace {
	// キャラクターCSVのパス
	constexpr char CharaCSVPath[] = "Data\\CSV\\CharaStatus.csv";
}


Player::Player()
	: m_pGJK		( nullptr )
	, m_MoveSpeed	( Time::GetDeltaTime() * 1000.f )
	, m_CamRevision	( 4.f )

	, m_AutoAim		( false )
	, m_Homing		( false )
	, m_WallHack	( false )
	, m_TriggerHappy( false )
{
	// 初期化
	m_CharaInfo.HP = m_CharaInfo.MaxHP;
	m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;

	// キャラクターCSVの情報保存用
	std::unordered_map<std::string, std::string> m_StateList;
	// キャラクターCSVの情報取得
	m_StateList = FileManager::CSVLoad(CharaCSVPath);
	
	// 空でない場合は、外部で調整するべき変数の値を入れていく
	if (!m_StateList.empty())
	{
		m_MoveSpeed		= StrToFloat(m_StateList["PlayerMoveSpeed"]) * Time::GetDeltaTime();
		m_CamRevision	= StrToFloat(m_StateList["CameraRevision"]);
	}
}
Player::~Player()
{
}


//============================================================================
//		更新処理
//============================================================================
void Player::Update(std::unique_ptr<Enemy>& chara)
{
	// 毎フレームリセットする
	m_SumVec = ZEROVEC3;
	m_Hit = false;

	// カメラに向きを合わせる
	m_vRotation.y = Camera::GetInstance()->GetRot().y;
	
	// 0以上のものがある場合カウントをする
	if ( m_DashTime >= 0.f )	{ m_DashTime -= Time::GetDeltaTime(); }
	if ( m_ReloadTime >= 0.f )	{
		m_ReloadTime -= Time::GetDeltaTime(); 
		// リロード終了時にSEを鳴らす
		if (m_ReloadTime < 0.f) {
			SoundManager::GetInstance()->Play(SoundManager::enList::SE_ReloadEnd);
		}
	}
	if ( m_DashCoolTime >= 0.f )	{ m_DashCoolTime	-= Time::GetDeltaTime(); }
	if ( m_BulletCoolTime >= 0.f )	{ m_BulletCoolTime	-= Time::GetDeltaTime(); }

	// 入力処理
	KeyInput(chara);

	// 入力処理後にカメラ座標をセット
	if (!Camera::GetInstance()->GetMoveCamera()) {
		// プレイヤー位置 + プレイヤーの高さを含んだ座標を渡す
		D3DXVECTOR3 campos = m_vPosition;
		campos.y += m_CamRevision;
		Camera::GetInstance()->SetPosition(campos);
	}

	// ホーミングのチートが有効な場合
	if (m_Homing) {
		for (size_t i = 0; i < m_pBullets.size(); ++i) 
		{
			D3DXVECTOR3 shootdir, EnemyPos;

			// 敵座標を取得(原点だと地面に当たるため少し浮かす)
			EnemyPos = chara->GetPos();
			EnemyPos.y += 0.5f;

			// 弾座標から敵座標のベクトルを出す
			shootdir = EnemyPos - m_pBullets[i]->GetPos();
			D3DXVec3Normalize(&shootdir, &shootdir); // 正規化

			// 弾の初期位置,移動方向の単位ベクトル,速度を設定
			m_pBullets[i]->Init( m_pBullets[i]->GetPos(), shootdir, m_BulletSpeed);
		}
	}

	// キャラクターの更新処理
	Character::Update();

	// ImGuiを用いたデバッグ関数
	if (ISDEBUG) { PlayerImGui(); }
}


//============================================================================
//		描画処理
//============================================================================
void Player::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	// プレイヤーは描画しない
	// Character::Draw( View, Proj, Light );

	// エフェクト事に必要なハンドルを用意
	static ::EsHandle hEffect = -1;

	D3DXVECTOR4 color = { 0.f, 0.f, 1.f, 1.f };

	// 弾とそのエフェクトの描画
	for (size_t i = 0; i < m_pBullets.size(); ++i) { 
		hEffect = Effect::Play(Effect::BulletSmoke, m_pBullets[i]->GetPos());
		m_pBullets[i]->Draw(View, Proj, Light);

		// ウォールハック中はメッシュ線が出現
		if (m_WallHack) {
			DirectX11::GetInstance()->SetDepth(false);
			m_pMeshLine->DrawMeshWireframeFromVertices(*m_pBullets[i]->GetMesh(), *m_pBullets[i], View, Proj, color, 5.f);
			m_pMeshLine->Render(View, Proj);
			DirectX11::GetInstance()->SetDepth(true);
		}
	}

	// 銃の描画
	m_pGun->Draw(View, Proj, Light);
}


//============================================================================
//		当たり判定処理
//============================================================================
void Player::Collision(std::unique_ptr<Enemy>& egg, Collider floor, Collider cylinder)
{
	// 距離チェック用変数
	float DistanceToFloorY		= 5.f;		// 床とのY座標距離
	float DistanceToCylinder	= 100.f;	// 柱
	float DistanceToEnemy		= 30.f;		// 敵
	Collider Bullet,enemyegg;

	// エフェクト事に必要なハンドルを用意
	static ::EsHandle hEffect = -1;

	// 敵データを取得
	enemyegg.SetVertex( egg->GetObjectInfo(), egg->GetMesh()->GetVertices());

	// 弾の判定
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		// 各中心点の取得
		D3DXVECTOR3 bulletCenter   = m_pBullets[i]->GetPos();
		D3DXVECTOR3 floorCenter    = floor.GetCenter();
		D3DXVECTOR3 cylinderCenter = cylinder.GetCenter();
		D3DXVECTOR3 enemyCenter    = enemyegg.GetCenter();
		
		// 距離計算
		D3DXVECTOR3 diffCylinder = bulletCenter - cylinderCenter;
		D3DXVECTOR3 diffEnemy    = bulletCenter - enemyCenter;
		
		float diffY				= fabsf(bulletCenter.y - floorCenter.y);
		float distToCylinder	= D3DXVec3Length(&diffCylinder);
		float distToEnemy		= D3DXVec3Length(&diffEnemy);

		// 弾データを取得
		Bullet.SetVertex( m_pBullets[i]->GetObjectInfo(), m_pMeshBullet->GetVertices());
		
		// 当たり判定情報用の変数を宣言
		CollisionPoints pointsbc, pointsbf, pointsbe;

		// 距離が近いときだけ判定
		if (diffY < DistanceToFloorY)
			pointsbf = m_pGJK->GJKC(Bullet, floor);

		if (distToCylinder < DistanceToCylinder)
			pointsbc = m_pGJK->GJKC(Bullet, cylinder);

		if (distToEnemy < DistanceToEnemy)
			pointsbe = m_pGJK->GJKC(Bullet, enemyegg);

		// 柱や床にあたった場合削除
		if (pointsbc.Col || pointsbf.Col)
		{
			// エフェクトの再生
			hEffect = Effect::Play(Effect::Dust, m_pBullets[i]->GetPos() - m_pBullets[i]->GetMoveVec() * 2);

			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}

		// エネミーと弾が当たった場合
		if ( pointsbe.Col ) {
			// ヘッドショット判定(気室判定)
			if (m_pBullets[i]->GetPos().y < egg->GetPos().y + m_EggAirRoomY) {
				// HPをクリティカルダメージ分減らす
				egg->CritDamage();
				// エフェクトの再生
				hEffect = Effect::Play(Effect::CritHit, egg->GetPos());
				// 命中種類の設定
				m_HitKind = HitKind::Crit;
				// クリティカル命中音を鳴らす
				SoundManager::GetInstance()->Play(SoundManager::enList::SE_CritHit);
			}
			else {
				// HPを胴体ダメージ分減らす
				egg->BodyDamage();
				// エフェクトがズレていたのでずらしてからエフェクトの再生
				D3DXVECTOR3 enemypos = egg->GetPos();
				enemypos.y += 2.f;
				hEffect = Effect::Play(Effect::Hit, enemypos);
				// 命中種類の設定
				m_HitKind = HitKind::Hit;
				// 命中音を鳴らす
				SoundManager::GetInstance()->Play(SoundManager::enList::SE_Hit);
			}

			// 命中した
			m_Hit = true;

			// 当たったあとは削除
			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}
	}
}


//-----------------------------------------------------------------------------
//		キー入力処理
//-----------------------------------------------------------------------------
void Player::KeyInput(std::unique_ptr<Enemy>& chara)
{

	Key* Key = DirectInput::GetInstance()->CDKeyboard();

	// 移動
	MoveProcess(chara);

	// ダッシュ
	DashProcess();

	//----------------------------------------------------------------------
	//		ジャンプ処理
	//----------------------------------------------------------------------
	if (m_Landing) { m_JumpPower = 0; }
	if (Key->IsKeyAction(DIK_SPACE) && m_CanJump && m_Landing) {
		m_JumpPower = m_JumpPowerMax;
		m_CanJump = m_Landing = false;
		// 処理順により着地判定が出てしまうため、押下時に少し上げる
		m_vPosition.y += m_JumpPower * Time::GetDeltaTime();
		SoundManager::GetInstance()->Play(SoundManager::enList::SE_Jump);
	}

	// 射撃
	ShotProcess();

	//----------------------------------------------------------------------
	//		Rでリロード
	//----------------------------------------------------------------------
	if (Key->IsKeyAction(DIK_R)) {
		m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;
		m_ReloadTime = m_ReloadTimeMax;
		SoundManager::GetInstance()->Play(SoundManager::enList::SE_Reload);
	}

	//----------------------------------------------------------------------
	//		チート関連
	//----------------------------------------------------------------------
	if (Key->IsKeyAction(DIK_1)) { m_AutoAim = !m_AutoAim; }
	if (Key->IsKeyAction(DIK_2)) { m_Homing = !m_Homing; }
	if (Key->IsKeyAction(DIK_3)) { m_WallHack = !m_WallHack; }
	if (Key->IsKeyAction(DIK_4)) { m_TriggerHappy = !m_TriggerHappy; RAINBOW_WINDOW = m_TriggerHappy; }


	// 合計のベクトル量分位置を更新
	m_vPosition += (m_SumVec + m_DashVec) * Time::GetDeltaTime();
}


//-----------------------------------------------------------------------------
//		移動処理
//-----------------------------------------------------------------------------
void Player::MoveProcess(std::unique_ptr<Enemy>& chara)
{
	// ダッシュ中は操作できないようにする
	if (0.f < m_DashTime) { return; }

	Key* Key = DirectInput::GetInstance()->CDKeyboard();

	// 操作が可能な間は初期化する
	m_DashVec = ZEROVEC3;

	D3DXVECTOR3 camDir;
	// カメラの向きベクトルを取得
	if (m_AutoAim) {
		camDir = chara->GetPos() - m_vPosition;
		camDir.y = 0.f;	// Y情報があると飛び始めるのでYの要素を抜く
		D3DXVec3Normalize(&camDir, &camDir); // 正規化
	}
	else {
		camDir = Camera::GetInstance()->GetCamDir();
		camDir.y = 0.f;	// Y情報があると飛び始めるのでYの要素を抜く
		D3DXVec3Normalize(&camDir, &camDir); // 正規化
	}

	// 移動する方向ベクトル
	D3DXVECTOR3 forward(ZEROVEC3);
	D3DXVECTOR3 left(ZEROVEC3);
	D3DXVECTOR3 upvec(0, 1, 0);

	// 左ベクトルを求める
	D3DXVec3Cross(&left, &camDir, &upvec);
	D3DXVec3Normalize(&left, &left);

	if ( Key->IsKeyDown(DIK_W)) { forward += camDir; }
	if ( Key->IsKeyDown(DIK_S)) { forward -= camDir; }
	if ( Key->IsKeyDown(DIK_A)) { forward += left; }
	if ( Key->IsKeyDown(DIK_D)) { forward -= left; }

	// 最終的なベクトル量を速度にかけ合計ベクトルに渡す
	m_SumVec += forward * m_MoveSpeed;
}


//-----------------------------------------------------------------------------
//		ダッシュ処理
//-----------------------------------------------------------------------------
void Player::DashProcess()
{
	Key* Key = DirectInput::GetInstance()->CDKeyboard();

	// クールタイムが終了していたらダッシュ可能に
	if (m_DashCoolTime <= 0.f) { m_CanDash = true; }

	// SHIFTでダッシュ
	if (!(Key->IsKeyAction(DIK_LSHIFT) && m_CanDash && m_Landing)) { return; }

	// ダッシュ時間の設定
	m_DashTime = m_DashTimeMax;
	// ダッシュクールタイムの設定
	m_DashCoolTime = m_DashCoolTimeMax;

	// 合計ベクトルに情報がない場合
	if (m_SumVec == ZEROVEC3) {
		// カメラの向きベクトルを取得
		D3DXVECTOR3 camDir = Camera::GetInstance()->GetCamDir();
		camDir.y = 0.f;	// Y情報があると飛び始めるのでYの要素を抜く
		D3DXVec3Normalize(&camDir, &camDir); // 正規化

		// カメラ方向 × 移動速度 × ダッシュ倍率のベクトルを出す
		m_DashVec = camDir * m_MoveSpeed * m_DashSpeed;
		m_CanDash = false;
	}
	else {
		// 合計ベクトルにダッシュ倍率を変えた値を出す
		m_DashVec = m_SumVec * m_DashSpeed;
		m_CanDash = false;
	}
	SoundManager::GetInstance()->Play(SoundManager::enList::SE_Dash);
}


//-----------------------------------------------------------------------------
//		発射処理
//-----------------------------------------------------------------------------
void Player::ShotProcess()
{
	Mouse* Mouse = DirectInput::GetInstance()->CDMouse();
	
	// クールタイムが終了していたら射撃可能
	if (m_BulletCoolTime <= 0.f) { m_CanShot = true; }

	// 左クリックが押されいない場合
	if (!(Mouse->IsLAction() || m_TriggerHappy && Mouse->IsLDown())) { return; }

	// エフェクト事に必要なハンドルを用意
	static ::EsHandle hEffect = -1;

	// カメラのレイHit座標から発射地点のベクトルを計算
	D3DXVECTOR3 shootdir = Camera::GetInstance()->GetRayHit() - m_pGun->GetShootPos();
	D3DXVec3Normalize(&shootdir, &shootdir);	// 正規化
	
	// 射撃条件が整っている場合
	if (m_CanShot && m_CharaInfo.Ammo != 0 && m_ReloadTime <= 0 || m_CanShot && m_TriggerHappy)
	{
		// 連射モードの場合クールタイムや残弾数の設定.
		if (m_TriggerHappy) {
			m_CanShot = false;
			m_BulletCoolTime = m_BulletCoolTimeMin;
		}
		else {
			m_CanShot = false;
			m_CharaInfo.Ammo--;
			m_BulletCoolTime = m_BulletCoolTimeMax;
		}

		// 弾を作成する
		m_pBullets.push_back(std::make_unique<Bullet>());

		m_pBullets.back()->AttachMesh(*m_pMeshBullet);	// メッシュを設定
		m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);	// nullにならないように見えない座標に初期設定
		m_pBullets.back()->SetScale(5.f, 5.f, 5.f);		// サイズを設定

		// 弾の初期位置,移動方向の単位ベクトル,速度を設定
		m_pBullets.back()->Init(
			m_pGun->GetShootPos(),
			shootdir,
			m_BulletSpeed);

		hEffect = Effect::Play(Effect::GunFire, m_pGun->GetShootPos());

		// 射撃音を鳴らす
		SoundManager::GetInstance()->Play(SoundManager::enList::SE_Shot);
	}
	else {
		// 空打ち音を鳴らす
		SoundManager::GetInstance()->Play(SoundManager::enList::SE_NoAmmo);
	}
}


//-----------------------------------------------------------------------------
//		デバッグ時に使用する関数
//-----------------------------------------------------------------------------
void Player::PlayerImGui()
{
	ImGui::Begin(IMGUI_JP("プレイヤー情報"));
	ImGui::Text("Dash%f", m_DashTime);
	ImGui::Text("Reload%f", m_ReloadTime);
	ImGui::Text("DashCool%f", m_DashCoolTime);
	ImGui::Text("BulletCool%f", m_BulletCoolTime);
	ImGui::End();
}
