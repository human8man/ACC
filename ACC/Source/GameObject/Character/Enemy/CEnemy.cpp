#include "CEnemy.h"
#include "DirectSound/CSoundManager.h"
#include "DirectInput/CDirectInput.h"
#include "Camera/CCamera.h"
#include "Effect/CEffect.h"
#include "Random/CRandom.h"
#include "FileManager/FileManager.h"

namespace {
	// キャラクターCSVのパス.
	constexpr char CharaCSVPath[] = "Data\\CSV\\CharaStatus.csv";
}


//=============================================================================
//		エネミークラス.
//=============================================================================
CEnemy::CEnemy()
	: m_pGJK				( nullptr )
	, m_MoveSpeed			( 0.2f )
	, m_SumVec				( ZEROVEC3 )
	
	, m_SelectMoveTime		( 0.f )
	, m_SelectMoveTimeMax	( CTime::GetInstance()->GetDeltaTime() * 60.f )
{
	m_CharaInfo.HP = m_CharaInfo.MaxHP;
	m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;


	// キャラクターCSVの情報保存用.
	std::unordered_map<std::string, std::string> m_StateList;
	// キャラクターCSVの情報取得.
	m_StateList = FileManager::CSVLoad(CharaCSVPath);

	// 空でない場合は、外部で調整するべき変数の値を入れていく.
	if (!m_StateList.empty()) {
		m_MoveSpeed			= StrToFloat(m_StateList["EnemyMoveSpeed"]);
		m_SelectMoveTimeMax = StrToFloat(m_StateList["SelectMoveTimeMax"]);
	}
}
CEnemy::~CEnemy()
{
}


//=============================================================================
//		更新処理.
//=============================================================================
void CEnemy::Update(std::unique_ptr<CPlayer>& chara)
{
	// 毎フレームリセットする.
	m_SumVec = ZEROVEC3;
	// 適当に回す.
	m_vRotation.y += 2.f;
	
	// クールタイム処理.
	if ( m_DashTime			>= 0.f) { m_DashTime		-= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_ReloadTime		>= 0.f) { m_ReloadTime		-= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_DashCoolTime		>= 0.f) { m_DashCoolTime	-= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_BulletCoolTime	>= 0.f) { m_BulletCoolTime	-= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_SelectMoveTime	>= 0.f) { m_SelectMoveTime	-= CTime::GetInstance()->GetDeltaTime(); }

	// 行動をまとめた関数.
	Act(chara);

	CCharacter::Update();
}


//=============================================================================
//		描画処理.
//=============================================================================
void CEnemy::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light)
{
	CCharacter::Draw( View, Proj, Light );

	// 弾の描画.
	for (size_t i = 0; i < m_pBullets.size(); ++i) { m_pBullets[i]->Draw(View, Proj, Light); }

	// 銃の描画.
	m_pGun->Draw(View, Proj, Light);
}


//=============================================================================
//		当たり判定処理.
//=============================================================================
void CEnemy::Collision(std::unique_ptr<CPlayer>& egg, Collider floor, Collider cylinder)
{
	Collider Bullet, enemyegg;

	// エフェクトに必要なハンドルを用意.
	static ::EsHandle hEffect = -1;

	// プレイヤーデータを取得.
	enemyegg.SetVertex( egg->GetObjeInfo(), egg->GetMesh()->GetVertices());

	// 弾の判定.
	for (size_t i = 0; i < m_pBullets.size(); ++i) 
	{
		// 弾データを取得.
		Bullet.SetVertex( m_pBullets[i]->GetObjeInfo(), m_pMeshBullet->GetVertices());


		// 当たり判定情報用の変数を宣言.
		CollisionPoints pointsbc, pointsbf, pointsbe;
		pointsbc = m_pGJK->GJK(Bullet, cylinder);
		pointsbf = m_pGJK->GJK(Bullet, floor);
		pointsbe = m_pGJK->GJK(Bullet, enemyegg);


		// 柱や床にあたった場合削除.
		if (pointsbc.Col || pointsbf.Col) {
			// エフェクトの再生.
			hEffect = CEffect::Play(CEffect::Dust, m_pBullets[i]->GetPos() - m_pBullets[i]->GetMoveVec() * 2);

			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}


		// プレイヤーに弾が当たった場合.
		if (pointsbe.Col) 
		{
			// ヘッドショット判定(気室判定).
			if (m_pBullets[i]->GetPos().y < egg->GetPos().y + m_EggAirRoomY) {
				// HPをクリティカルダメージ分減らす.
				egg->CritDamage();
				CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_DamageCrit);
			}
			else {
				// HPを胴体ダメージ分減らす.
				egg->BodyDamage();
				CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_Damage);
			}

			// 命中した.
			m_Hit = true;

			// 命中後に削除.
			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}
	}
}


//-----------------------------------------------------------------------------
//		行動をまとめ関数.
//-----------------------------------------------------------------------------
void CEnemy::Act(std::unique_ptr<CPlayer>& chara)
{
	CRandom random;
	
	//-------------------------------------------------------------------------
	//		ランダムで移動.
	//-------------------------------------------------------------------------

	// 次の行動決定までの時間が 0 以下の場合.
	if (m_SelectMoveTime <= 0.f) {
		m_MoveKind = random.GetRandomInt(0, MoveKind::max - 1);
		m_SelectMoveTime = m_SelectMoveTimeMax;
	}

	// ダッシュ中は操作できないようにする.
	if (m_DashTime <= 0.f) {
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

		if (m_MoveKind == MoveKind::Straight)	{ forward += camDir; }
		if (m_MoveKind == MoveKind::Back)		{ forward -= camDir; }
		if (m_MoveKind == MoveKind::Left)		{ forward += left; }
		if (m_MoveKind == MoveKind::Right)		{ forward -= left; }
		if (m_MoveKind == MoveKind::Wait)		{ forward = ZEROVEC3; }

		// 最終的なベクトル量を速度にかけ合計ベクトルに渡す.
		m_SumVec += forward * m_MoveSpeed;
	}


	//-------------------------------------------------------------------------
	//		ランダムでダッシュ.
	//-------------------------------------------------------------------------

	// クールタイムが終了していたらダッシュ可能に.
	if (m_DashCoolTime <= 0.f) { m_CanDash = true; }

	// ランダムにダッシュのタイミングを決める.
	bool dash = (random.GetRandomInt(0, 120) == 0);

	// 条件が整っている場合.
	if (dash && m_CanDash) {
		// ダッシュ時間の設定.
		m_DashTime = m_DashTimeMax;
		// ダッシュクールタイムの設定.
		m_DashCoolTime = m_DashCoolTimeMax;

		// 合計ベクトルに情報がない場合.
		if (m_SumVec == ZEROVEC3) {
			D3DXVECTOR3 camDir;
			// 方向ベクトルを更新.
			camDir.x = cos(D3DXToRadian(m_vRotation.y)) * cos(D3DXToRadian(m_vRotation.x));
			camDir.y = sin(D3DXToRadian(m_vRotation.x));
			camDir.z = sin(D3DXToRadian(m_vRotation.y)) * cos(D3DXToRadian(m_vRotation.x));

			// カメラの向きベクトルを取得.
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


	//-------------------------------------------------------------------------
	//		ランダムジャンプ処理.
	//-------------------------------------------------------------------------

	// ランダムにジャンプのタイミングを決める.
	bool jump = (random.GetRandomInt(0, 120) == 0);

	// ランダムにジャンプのタイミングを決める.
	if (jump && m_CanJump) {
		m_JumpPower = m_JumpPowerMax;
		m_CanJump = false;
	}
	// ジャンプ力をY値に加算.
	m_vPosition.y += m_JumpPower;


	// カメラのレイHit座標から発射地点のベクトルを計算.
	D3DXVECTOR3 shootdir = chara->GetPos() - m_pGun->GetShootPos();
	shootdir.y += 0.2f; // 補正値を入れる.
	D3DXVec3Normalize(&shootdir, &shootdir);	// 正規化.


	//-------------------------------------------------------------------------
	//		ランダムで射撃.
	//-------------------------------------------------------------------------

	// クールタイムが終了していたら射撃可能.
	if (m_BulletCoolTime <= 0.f) { m_CanShot = true; }

	// ランダムに射撃タイミングを決める.
	bool shot = (random.GetRandomInt(0, 4) == 0);

	// 射撃条件が整っていた場合.
	if (shot && m_CanShot && m_CharaInfo.Ammo != 0 && m_ReloadTime <= 0) 
	{
		m_CanShot = false;						// 射撃不可にする.
		m_CharaInfo.Ammo--;						// 残弾数を減らす.
		m_BulletCoolTime = m_BulletCoolTimeMax;	// 再発射までの時間を設定.

		m_pBullets.push_back(std::make_unique<CBullet>());

		m_pBullets.back()->AttachMesh(*m_pMeshBullet);	// メッシュを設定.
		m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);	// nullにならないように見えない座標に初期設定.
		m_pBullets.back()->SetScale(5.f, 5.f, 5.f);		// サイズを設定.

		// 弾の初期位置,移動方向の単位ベクトル,速度を設定.
		m_pBullets.back()->Init(
			m_pGun->GetShootPos(),
			shootdir,
			m_BulletSpeed);
	}

	// 弾切れででリロード.
	if (m_CharaInfo.Ammo == 0) {
		m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;	// 残弾数に最大弾薬数を設定.
		m_ReloadTime = m_ReloadTimeMax;			// リロード時間の設定.
	}

	// 合計のベクトル量分位置を更新.
	m_vPosition += m_SumVec + DashVec;
}