#include "CCharacter.h"
#include "Camera/CCamera.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"


namespace {
	// キャラクターCSVのパス.
	constexpr char CharaCSVPath[] = "Data\\CSV\\CharaStatus.csv";
}


//============================================================================
//		キャラクタークラス.
//============================================================================
CCharacter::CCharacter()
	: m_pRayY				( nullptr )
	, m_pCrossRay			( nullptr )

	, m_pMeshGun			( nullptr )
	, m_pMeshBullet			( nullptr )
	, m_pMeshLine			( nullptr )
	, m_pGun				( nullptr )
	, m_pBullets			()

	, m_BodyDamage			( 1 )
	, m_CritDamage			( 2 )
	, m_Damage				( false )

	, m_GunRadius			( 1.f )
	, m_GunRotRevision		( -1.5f )
	, m_GunPosRevision		( -30.f )
	, m_Gravity				( 1.3f )

	, m_ReloadTime			( 0.f )
	, m_ReloadTimeMax		( CTime::GetDeltaTime() * 120.f )
	, m_BulletCoolTime		( 0.f )
	, m_BulletCoolTimeMax	( CTime::GetDeltaTime() * 20.f )
	, m_BulletSpeed			( 180.f )
	, m_CanShot				( true )

	, m_JumpPower			( 0.f )
	, m_JumpPowerMax		( 25.f )
	, m_CanJump				( false )
	, m_Landing				( false )

	, m_DashCoolTime		( 0.f )
	, m_DashCoolTimeMax		( CTime::GetDeltaTime() * 60.f )
	, m_DashTime			( 0.f )
	, m_DashTimeMax			( CTime::GetDeltaTime() * 30.f )
	, m_DashSpeed			( 1.8f )
	, m_CanDash				( true )
	, m_DashVec				( ZEROVEC3 )

	, m_EggAirRoomY			( 1.f )

	, m_CharaInfo			()
	, m_SumVec				(ZEROVEC3)
{
	// レイの設定.
	m_pRayY			= std::make_unique<RAY>();
	m_pCrossRay		= std::make_unique<CROSSRAY>();
	m_pRayY->Axis	= D3DXVECTOR3(0.f, -1.f, 0.f);	// 下向きの軸を設定.
	m_pRayY->Length	= 10.f;							// レイ長さを設定.

	// 弾と銃のメッシュ設定.
	m_pMeshGun		= std::make_unique<CStaticMesh>();
	m_pMeshBullet	= std::make_unique<CStaticMesh>();
	m_pMeshGun		->Init( _T( "Data\\Mesh\\Static\\Gun\\Gun.x" ) );
	m_pMeshBullet	->Init( _T( "Data\\Mesh\\Static\\Bullet\\bullet.x" ) );

	// メッシュ線の設定.
	m_pMeshLine = std::make_unique<CMeshLine>();
	m_pMeshLine->Init();

	// 銃の設定.
	m_pGun	= std::make_unique<CGun>();
	m_pGun	->AttachMesh( *m_pMeshGun );

	// キャラの初期情報.
	m_CharaInfo.MaxHP = 20;
	m_CharaInfo.MaxAmmo = 6;


	// キャラクターCSVの情報保存用.
	std::unordered_map<std::string, std::string> m_StateList;
	// キャラクターCSVの情報取得.
	m_StateList = FileManager::CSVLoad(CharaCSVPath);
	
	// 空でない場合は、外部で調整するべき変数の値を入れていく.
	if (!m_StateList.empty())
	{
		m_BodyDamage		= StrToInt(m_StateList["BodyDamage"]);
		m_CritDamage		= StrToInt(m_StateList["CritDamage"]);
		m_GunRadius			= StrToFloat(m_StateList["GunRadius"]);
		m_GunRotRevision	= StrToFloat(m_StateList["GunRotRevision"]);
		m_GunPosRevision	= StrToFloat(m_StateList["GunPosRevision"]);
		m_ReloadTimeMax		= StrToFloat( m_StateList["ReloadTimeMax"]) * CTime::GetInstance()->GetDeltaTime();
		m_BulletCoolTimeMax	= StrToFloat(m_StateList["BulletCoolTimeMax"]) * CTime::GetInstance()->GetDeltaTime();
		m_BulletSpeed		= StrToFloat(m_StateList["BulletSpeed"]);
		m_JumpPowerMax		= StrToFloat(m_StateList["JumpPowerMax"]);
		m_DashCoolTimeMax	= StrToFloat(m_StateList["DashCoolTimeMax"]) * CTime::GetInstance()->GetDeltaTime();
		m_DashTimeMax		= StrToFloat(m_StateList["DashTimeMax"]) * CTime::GetInstance()->GetDeltaTime();
		m_DashSpeed			= StrToFloat(m_StateList["DashSpeed"]);
		m_EggAirRoomY		= StrToFloat(m_StateList["EggAirRoomY"]);
		m_CharaInfo.MaxHP	= StrToInt(m_StateList["CharaHPMax"]);
		m_CharaInfo.MaxAmmo	= StrToInt(m_StateList["CharaAmmoMax"]);
	}
}
CCharacter::~CCharacter()
{
	m_pCrossRay.reset();
	m_pRayY.reset();
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		m_pBullets[i].reset();
	}
	m_pGun.reset();
	m_pMeshGun.reset();
	m_pMeshBullet.reset();
}


//============================================================================
//		更新処理.
//============================================================================
void CCharacter::Update()
{
	// ダメージフラグを毎フレーム初期化.
	m_Damage = false;

	// 銃をキャラの周りで回す処理.
	m_pGun->UpdateGunPos(
		m_vPosition,
		m_GunRadius,
		D3DXToRadian(m_vRotation.y + m_GunPosRevision));

	// 銃の角度をプレイヤーの向き + 補正値に設定.
	m_pGun->SetRot( 0.f, -D3DXToRadian(m_vRotation.y) + m_GunRotRevision, 0.f);

	// 弾の更新処理.
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		m_pBullets[i]->Update(); // 各弾の更新処理.

		// 一定時間経過した弾を削除.
		if (m_pBullets[i]->DeleteBullet()) {
			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}
	}
}


//============================================================================
//		描画処理.
//============================================================================
void CCharacter::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	CStaticMeshObject::Draw( View, Proj, Light );
}


//============================================================================
//		ジャンプ関係の計算.
//============================================================================
void CCharacter::JumpMath()
{
	// ジャンプ力を重力で減算し、Yに加算.
	m_JumpPower = m_JumpPower - m_Gravity * CTime::GetTimeScale();
	m_vPosition.y += m_JumpPower * CTime::GetDeltaTime();
}


//============================================================================
//		HPを胴体ダメージ分減らす.
//============================================================================
void CCharacter::BodyDamage()
{
	m_CharaInfo.HP -= m_BodyDamage;
	m_Damage = true;
}


//============================================================================
//		HPをクリティカルダメージ分減らす.
//============================================================================
void CCharacter::CritDamage()
{
	m_CharaInfo.HP -= m_CritDamage;
	m_Damage = true;
}


//============================================================================
//		最終移動ベクトルを渡す.
//============================================================================
const D3DXVECTOR3& CCharacter::GetMoveVec()
{
	return m_SumVec * CTime::GetDeltaTime();
}