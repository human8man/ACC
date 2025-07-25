#include "Enemy.h"
#include "Camera/Camera.h"
#include "Effect/Effect.h"
#include "Random/Random.h"
#include "DirectX/DirectX11.h"
#include "FileManager/FileManager.h"
#include "DirectInput/DirectInput.h"
#include "DirectSound/SoundManager.h"
#include "ImGui/CImGui.h"

namespace {
	// キャラクターCSVのパス
	constexpr char CharaCSVPath[] = "Data\\CSV\\CharaStatus.csv";
}


//=============================================================================
//		エネミークラス
//=============================================================================
Enemy::Enemy()
	: m_pGJK				( nullptr )
	, m_MoveSpeed			( Time::GetDeltaTime() * 1000.f )
	
	, m_WallHack			( false )
	, m_Hit					( false )
	, m_HitKind				( false )
	, m_SelectMoveTime		( 0.f )
	, m_SelectMoveTimeMax	( Time::GetDeltaTime() * 10.f )
	
	, m_MoveKind			( MoveKind::Wait )
	, m_MoveToPlayer		( 10 )
{
	m_CharaInfo.HP = m_CharaInfo.MaxHP;
	m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;

	// キャラクターCSVの情報保存用
	std::unordered_map<std::string, std::string> m_StateList;
	// キャラクターCSVの情報取得
	m_StateList = FileManager::CSVLoad(CharaCSVPath);

	// 空でない場合は、外部で調整するべき変数の値を入れていく
	if (!m_StateList.empty()) {
		m_MoveSpeed			= StrToFloat(m_StateList["EnemyMoveSpeed"]) * Time::GetDeltaTime();
		m_SelectMoveTimeMax = StrToFloat(m_StateList["SelectMoveTimeMax"]) * Time::GetDeltaTime();
		m_MoveToPlayer		= StrToInt(m_StateList["MoveToPlayer"]);
	}
}
Enemy::~Enemy()
{
}


//=============================================================================
//		更新処理
//=============================================================================
void Enemy::Update(std::unique_ptr<Player>& chara)
{
	// 毎フレームリセットする
	m_SumVec = ZEROVEC3;
	// プレイヤーに向きを合わせる
	float deltaX = chara->GetPos().x - m_vPosition.x;
	float deltaZ = chara->GetPos().z - m_vPosition.z;
	float targetAngleRad1 = std::atan2(deltaZ, deltaX);

	m_vRotation.y = targetAngleRad1;
	
	// クールタイム処理
	if ( m_DashTime			>= 0.f) { m_DashTime		-= Time::GetDeltaTime(); }
	if ( m_ReloadTime		>= 0.f) { m_ReloadTime		-= Time::GetDeltaTime(); }
	if ( m_DashCoolTime		>= 0.f) { m_DashCoolTime	-= Time::GetDeltaTime(); }
	if ( m_BulletCoolTime	>= 0.f) { m_BulletCoolTime	-= Time::GetDeltaTime(); }
	if ( m_SelectMoveTime	>= 0.f) { m_SelectMoveTime	-= Time::GetDeltaTime(); }

	// 行動をまとめた関数
	Act(chara);
	
	m_WallHack = chara->GetWallHack();

	Character::Update();
}


//=============================================================================
//		描画処理
//=============================================================================
void Enemy::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light)
{
	Character::Draw( View, Proj, Light );

	// エフェクト事に必要なハンドルを用意
	static ::EsHandle hEffect = -1;

	D3DXVECTOR4 color = { 1.f,0.f,0.f,1.f };

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


//-----------------------------------------------------------------------------
//		行動をまとめ関数
//-----------------------------------------------------------------------------
void Enemy::Act(std::unique_ptr<Player>& chara)
{
	Random random;
	
	//-------------------------------------------------------------------------
	//		移動決定処理
	//-------------------------------------------------------------------------
#pragma region 移動決定処理
	// 次の行動決定までの時間が 0 以下の場合
	if (m_SelectMoveTime <= 0.f) {

		// プレイヤーへの移動とランダム移動の確率計算
		if (random.GetRandomInt(0, 100) < m_MoveToPlayer) { 
			m_MoveKind = MoveKind::Straight; 
		}
		else { 
			m_MoveKind = random.GetRandomInt(0, MoveKind::max - 1); 
		}

		// 次の行動選択までのクールタイムを設定
		m_SelectMoveTime = m_SelectMoveTimeMax;
	}

	// ダッシュ中は操作できないようにする
	if (m_DashTime <= 0.f) {
		// 操作が可能な間は初期化する
		m_DashVec = ZEROVEC3;

		// 敵の向きベクトルを計算(プレイヤーが正面)
		D3DXVECTOR3 camDir = chara->GetPos() - m_vPosition;
		camDir.y = 0.f;	// Y情報があると飛び始めるのでYの要素を抜く
		D3DXVec3Normalize(&camDir, &camDir); // 正規化

		// 移動する方向ベクトル
		D3DXVECTOR3 forward(ZEROVEC3);
		D3DXVECTOR3 left(ZEROVEC3);
		D3DXVECTOR3 upvec(0.f, 1.f, 0.f);

		// 左ベクトルを求める
		D3DXVec3Cross(&left, &camDir, &upvec);
		D3DXVec3Normalize(&left, &left);

		if (m_MoveKind == MoveKind::Straight)	{ forward += camDir;	}
		if (m_MoveKind == MoveKind::Back)		{ forward -= camDir;	}
		if (m_MoveKind == MoveKind::Left)		{ forward += left;		}
		if (m_MoveKind == MoveKind::Right)		{ forward -= left;		}
		if (m_MoveKind == MoveKind::Wait)		{ forward = ZEROVEC3;	}

		// 最終的なベクトル量を速度にかけ合計ベクトルに渡す
		m_SumVec += forward * m_MoveSpeed;
	}
#pragma endregion


	//-------------------------------------------------------------------------
	//		ダッシュ処理
	//-------------------------------------------------------------------------
#pragma region ダッシュ処理
	// クールタイムが終了していたらダッシュ可能に
	if (m_DashCoolTime <= 0.f) { m_CanDash = true; }

	// ランダムにダッシュのタイミングを決める
	bool dash = (random.GetRandomInt(0, 120) == 0);

	// 条件が整っている場合
	if (dash && m_CanDash && m_Landing) {
		// ダッシュ時間の設定
		m_DashTime = m_DashTimeMax;
		// ダッシュクールタイムの設定
		m_DashCoolTime = m_DashCoolTimeMax;

		// 合計ベクトルに情報がない場合
		if (m_SumVec == ZEROVEC3) {
			D3DXVECTOR3 camDir;
			// 方向ベクトルを更新
			camDir.x = cos(D3DXToRadian(m_vRotation.y)) * cos(D3DXToRadian(m_vRotation.x));
			camDir.y = sin(D3DXToRadian(m_vRotation.x));
			camDir.z = sin(D3DXToRadian(m_vRotation.y)) * cos(D3DXToRadian(m_vRotation.x));

			// カメラの向きベクトルを取得
			camDir.y = 0.f;	// Y情報があると飛び始めるのでYの要素を抜く
			D3DXVec3Normalize(&camDir, &camDir); // 正規化
			m_DashVec = camDir * m_MoveSpeed * m_DashSpeed;
			m_CanDash = false;
		}
		else {
			m_DashVec = m_SumVec * m_DashSpeed;
			m_CanDash = false;
		}
	}
#pragma endregion


	//-------------------------------------------------------------------------
	//		ジャンプ処理
	//-------------------------------------------------------------------------
#pragma region ジャンプ処理

	// ランダムにジャンプのタイミングを決める
	bool jump = (random.GetRandomInt(0, 120) == 0);

	// ランダムにジャンプのタイミングを決める
	if (m_Landing) { m_JumpPower = 0; }
	if (jump && m_CanJump && m_Landing) {
		m_JumpPower = m_JumpPowerMax;
		m_CanJump = m_Landing = false;
		// 処理順により着地判定が出てしまうため、ジャンプ時に少し上げる
		m_vPosition.y += m_JumpPower * Time::GetDeltaTime();
	}
#pragma endregion


	//-------------------------------------------------------------------------
	//		射撃処理
	//-------------------------------------------------------------------------
#pragma region 射撃処理

	// カメラのレイHit座標から発射地点のベクトルを計算
	D3DXVECTOR3 shootdir = chara->GetPos() - m_pGun->GetShootPos();
	shootdir.y += 0.5f; // 補正値を入れる
	D3DXVec3Normalize(&shootdir, &shootdir);	// 正規化

	// クールタイムが終了していたら射撃可能
	if (m_BulletCoolTime <= 0.f) { m_CanShot = true; }

	// ランダムに射撃タイミングを決める
	bool shot = (random.GetRandomInt(0, 10) == 0);

	// 射撃条件が整っていた場合
	if (shot && m_CanShot && m_CharaInfo.Ammo != 0 && m_ReloadTime <= 0) 
	{
		m_CanShot = false;						// 射撃不可にする
		m_CharaInfo.Ammo--;						// 残弾数を減らす
		m_BulletCoolTime = m_BulletCoolTimeMax;	// 再発射までの時間を設定

		m_pBullets.push_back(std::make_unique<Bullet>());

		m_pBullets.back()->AttachMesh(*m_pMeshBullet);	// メッシュを設定
		m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);	// nullにならないように見えない座標に初期設定
		m_pBullets.back()->SetScale(5.f, 5.f, 5.f);		// サイズを設定

		// 弾の初期位置,移動方向の単位ベクトル,速度を設定
		m_pBullets.back()->Init(
			m_pGun->GetShootPos(),
			shootdir,
			m_BulletSpeed);
	}

	// 弾切れででリロード
	if (m_CharaInfo.Ammo == 0) {
		m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;	// 残弾数に最大弾薬数を設定
		m_ReloadTime = m_ReloadTimeMax;			// リロード時間の設定
	}

	// 合計のベクトル量分位置を更新
	m_vPosition += (m_SumVec + m_DashVec) * Time::GetDeltaTime();
#pragma endregion
}