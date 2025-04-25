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

	, m_GunRadius			( 1.f )
	, m_GunRotRevision		( -1.5f )
	, m_GunPosRevision		( -30.f )
	, m_Gravity				( 0.0098f )
	, m_GravityValue		( 0.0098f )

	, m_ReloadTime			( 0.f )
	, m_ReloadTimeMax		( CTime::GetInstance()->GetDeltaTime() * 120.f )
	, m_BulletCoolTime		( 0.f )
	, m_BulletCoolTimeMax	( CTime::GetInstance()->GetDeltaTime() * 60.f )
	, m_BulletSpeed			( 2.3f )
	, m_CanShot				( true )

	, m_JumpPower			( 0.f )
	, m_JumpPowerMax		( 0.784f )
	, m_CanJump				( false )

	, m_DashCoolTime		( 0.f )
	, m_DashCoolTimeMax		( CTime::GetInstance()->GetDeltaTime() * 60.f )
	, m_DashTime			( 0.f )
	, m_DashTimeMax			( CTime::GetInstance()->GetDeltaTime() * 30.f )
	, m_DashSpeed			( 2.6f )
	, m_CanDash				( true )
	, DashVec				( ZEROVEC3 )

	, m_EggAirRoomY			( 1.f )

	, m_CharaInfo			()
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
		m_GravityValue		= StrToFloat(m_StateList["GravityValue"]);
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
	// スタティックメッシュオブジェクトの更新処理(たぶんいらない).
	CStaticMeshObject::Update();

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
