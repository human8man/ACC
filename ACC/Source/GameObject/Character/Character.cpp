#include "Character.h"
#include "Camera/Camera.h"
#include "Time/Time.h"
#include "FileManager/FileManager.h"


namespace {
	// �L�����N�^�[CSV�̃p�X
	constexpr char CharaCSVPath[] = "Data\\CSV\\CharaStatus.csv";
}


Character::Character()
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
	, m_ReloadTimeMax		( Time::GetDeltaTime() * 120.f )
	, m_BulletCoolTime		( 0.f )
	, m_BulletCoolTimeMax	( Time::GetDeltaTime() * 20.f )
	, m_BulletCoolTimeMin	( Time::GetDeltaTime() * 2.f )
	, m_BulletSpeed			( 120.f )
	, m_CanShot				( true )

	, m_JumpPower			( 0.f )
	, m_JumpPowerMax		( 25.f )
	, m_CanJump				( false )
	, m_Landing				( false )

	, m_DashCoolTime		( 0.f )
	, m_DashCoolTimeMax		( Time::GetDeltaTime() * 60.f )
	, m_DashTime			( 0.f )
	, m_DashTimeMax			( Time::GetDeltaTime() * 30.f )
	, m_DashSpeed			( 1.8f )
	, m_CanDash				( true )
	, m_DashVec				( ZEROVEC3 )

	, m_EggAirRoomY			( 1.f )

	, m_CharaInfo			()
	, m_SumVec				(ZEROVEC3)

	, m_RotGun				( false ) 
	, m_RotSpeed			( 0.5f )
{
	// ���C�̐ݒ�
	m_pRayY			= std::make_unique<RAY>();
	m_pCrossRay		= std::make_unique<CROSSRAY>();
	m_pRayY->Axis	= D3DXVECTOR3(0.f, -1.f, 0.f);	// �������̎���ݒ�
	m_pRayY->Length	= 10.f;							// ���C������ݒ�

	// �e�Əe�̃��b�V���ݒ�
	m_pMeshGun		= std::make_unique<StaticMesh>();
	m_pMeshBullet	= std::make_unique<StaticMesh>();
	m_pMeshGun		->Init( _T( "Data\\Mesh\\Static\\Gun\\Gun.x" ) );
	m_pMeshBullet	->Init( _T( "Data\\Mesh\\Static\\Bullet\\bullet.x" ) );

	// ���b�V�����̐ݒ�
	m_pMeshLine = std::make_unique<MeshLine>();
	m_pMeshLine->Init();

	// �e�̐ݒ�
	m_pGun	= std::make_unique<Gun>();
	m_pGun	->AttachMesh( *m_pMeshGun );

	// �L�����̏������
	m_CharaInfo.MaxHP = 20;
	m_CharaInfo.MaxAmmo = 6;


	// �L�����N�^�[CSV�̏��ۑ��p
	std::unordered_map<std::string, std::string> m_StateList;
	// �L�����N�^�[CSV�̏��擾
	m_StateList = FileManager::CSVLoad(CharaCSVPath);
	
	// ��łȂ��ꍇ�́A�O���Œ�������ׂ��ϐ��̒l�����Ă���
	if (!m_StateList.empty())
	{
		m_BodyDamage		= StrToInt(m_StateList["BodyDamage"]);
		m_CritDamage		= StrToInt(m_StateList["CritDamage"]);
		m_GunRadius			= StrToFloat(m_StateList["GunRadius"]);
		m_GunRotRevision	= StrToFloat(m_StateList["GunRotRevision"]);
		m_GunPosRevision	= StrToFloat(m_StateList["GunPosRevision"]);
		m_ReloadTimeMax		= StrToFloat( m_StateList["ReloadTimeMax"]) * Time::GetDeltaTime();
		m_BulletCoolTimeMax	= StrToFloat(m_StateList["BulletCoolTimeMax"]) * Time::GetDeltaTime();
		m_BulletSpeed		= StrToFloat(m_StateList["BulletSpeed"]);
		m_JumpPowerMax		= StrToFloat(m_StateList["JumpPowerMax"]);
		m_DashCoolTimeMax	= StrToFloat(m_StateList["DashCoolTimeMax"]) * Time::GetDeltaTime();
		m_DashTimeMax		= StrToFloat(m_StateList["DashTimeMax"]) * Time::GetDeltaTime();
		m_DashSpeed			= StrToFloat(m_StateList["DashSpeed"]);
		m_EggAirRoomY		= StrToFloat(m_StateList["EggAirRoomY"]);
		m_CharaInfo.MaxHP	= StrToInt(m_StateList["CharaHPMax"]);
		m_CharaInfo.MaxAmmo	= StrToInt(m_StateList["CharaAmmoMax"]);
	}
}
Character::~Character()
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
//		�X�V����
//============================================================================
void Character::Update()
{
	// �_���[�W�t���O�𖈃t���[��������
	m_Damage = false;

	// �e���L�����̎���ŉ񂷏���
	m_pGun->UpdateGunPos(
		m_vPosition,
		m_GunRadius,
		D3DXToRadian(m_vRotation.y + m_GunPosRevision));

	if (!m_RotGun) {
		// �e�̊p�x���v���C���[�̌��� + �␳�l�ɐݒ�
		m_pGun->SetRot( 0.f, -D3DXToRadian(m_vRotation.y) + m_GunRotRevision, 0.f);
	}
	else {
		m_pGun->SetRot(0.f, m_pGun->GetRot().y + m_RotSpeed, 0.f);
	}

	// �e�̍X�V����
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		m_pBullets[i]->Update(); // �e�e�̍X�V����

		// ��莞�Ԍo�߂����e���폜
		if (m_pBullets[i]->DeleteBullet()) {
			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}
	}
}


//============================================================================
//		�`�揈��
//============================================================================
void Character::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	StaticMeshObject::Draw( View, Proj, Light );
}


//============================================================================
//		�W�����v�֌W�̌v�Z
//============================================================================
void Character::JumpMath()
{
	// �W�����v�͂��d�͂Ō��Z���AY�ɉ��Z
	m_JumpPower = m_JumpPower - m_Gravity * Time::GetTimeScale();
	m_vPosition.y += m_JumpPower * Time::GetDeltaTime();
}


//============================================================================
//		HP�𓷑̃_���[�W�����炷
//============================================================================
void Character::BodyDamage()
{
	m_CharaInfo.HP -= m_BodyDamage;
	m_Damage = true;
}


//============================================================================
//		HP���N���e�B�J���_���[�W�����炷
//============================================================================
void Character::CritDamage()
{
	m_CharaInfo.HP -= m_CritDamage;
	m_Damage = true;
}


//============================================================================
//		�ŏI�ړ��x�N�g����n��
//============================================================================
D3DXVECTOR3 Character::GetMoveVec()
{
	return m_SumVec * Time::GetDeltaTime();
}