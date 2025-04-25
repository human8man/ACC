#include "CCharacter.h"
#include "Camera/CCamera.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"


namespace {
	// �L�����N�^�[CSV�̃p�X.
	constexpr char CharaCSVPath[] = "Data\\CSV\\CharaStatus.csv";
}


//============================================================================
//		�L�����N�^�[�N���X.
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
	// ���C�̐ݒ�.
	m_pRayY			= std::make_unique<RAY>();
	m_pCrossRay		= std::make_unique<CROSSRAY>();
	m_pRayY->Axis	= D3DXVECTOR3(0.f, -1.f, 0.f);	// �������̎���ݒ�.
	m_pRayY->Length	= 10.f;							// ���C������ݒ�.

	// �e�Əe�̃��b�V���ݒ�.
	m_pMeshGun		= std::make_unique<CStaticMesh>();
	m_pMeshBullet	= std::make_unique<CStaticMesh>();
	m_pMeshGun		->Init( _T( "Data\\Mesh\\Static\\Gun\\Gun.x" ) );
	m_pMeshBullet	->Init( _T( "Data\\Mesh\\Static\\Bullet\\bullet.x" ) );

	// ���b�V�����̐ݒ�.
	m_pMeshLine = std::make_unique<CMeshLine>();
	m_pMeshLine->Init();

	// �e�̐ݒ�.
	m_pGun	= std::make_unique<CGun>();
	m_pGun	->AttachMesh( *m_pMeshGun );

	// �L�����̏������.
	m_CharaInfo.MaxHP = 20;
	m_CharaInfo.MaxAmmo = 6;


	// �L�����N�^�[CSV�̏��ۑ��p.
	std::unordered_map<std::string, std::string> m_StateList;
	// �L�����N�^�[CSV�̏��擾.
	m_StateList = FileManager::CSVLoad(CharaCSVPath);
	
	// ��łȂ��ꍇ�́A�O���Œ�������ׂ��ϐ��̒l�����Ă���.
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
//		�X�V����.
//============================================================================
void CCharacter::Update()
{
	// �X�^�e�B�b�N���b�V���I�u�W�F�N�g�̍X�V����(���Ԃ񂢂�Ȃ�).
	CStaticMeshObject::Update();

	// �e���L�����̎���ŉ񂷏���.
	m_pGun->UpdateGunPos(
		m_vPosition,
		m_GunRadius,
		D3DXToRadian(m_vRotation.y + m_GunPosRevision));

	// �e�̊p�x���v���C���[�̌��� + �␳�l�ɐݒ�.
	m_pGun->SetRot( 0.f, -D3DXToRadian(m_vRotation.y) + m_GunRotRevision, 0.f);

	// �e�̍X�V����.
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		m_pBullets[i]->Update(); // �e�e�̍X�V����.

		// ��莞�Ԍo�߂����e���폜.
		if (m_pBullets[i]->DeleteBullet()) {
			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}
	}
}


//============================================================================
//		�`�揈��.
//============================================================================
void CCharacter::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	CStaticMeshObject::Draw( View, Proj, Light );
}
