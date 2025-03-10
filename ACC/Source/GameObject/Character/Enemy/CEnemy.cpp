#include "CEnemy.h"
#include "DirectSound/CSoundManager.h"
#include "DirectInput/CDirectInput.h"
#include "Camera/CCamera.h"
#include "Effect/CEffect.h"
#include "Random/CRandom.h"
#include "FileManager/FileManager.h"

namespace {
	// �L�����N�^�[CSV�̃p�X.
	constexpr char CharaCSVPath[] = "Data\\CSV\\CharaStatus.csv";
}


//=============================================================================
//		�G�l�~�[�N���X.
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


	// �L�����N�^�[CSV�̏��ۑ��p.
	std::unordered_map<std::string, std::string> m_StateList;
	// �L�����N�^�[CSV�̏��擾.
	m_StateList = FileManager::CSVLoad(CharaCSVPath);

	// ��łȂ��ꍇ�́A�O���Œ�������ׂ��ϐ��̒l�����Ă���.
	if (!m_StateList.empty()) {
		m_MoveSpeed			= StrToFloat(m_StateList["EnemyMoveSpeed"]);
		m_SelectMoveTimeMax = StrToFloat(m_StateList["SelectMoveTimeMax"]);
	}
}
CEnemy::~CEnemy()
{
}


//=============================================================================
//		�X�V����.
//=============================================================================
void CEnemy::Update(std::unique_ptr<CPlayer>& chara)
{
	// ���t���[�����Z�b�g����.
	m_SumVec = ZEROVEC3;
	// �K���ɉ�.
	m_vRotation.y += 2.f;
	
	// �N�[���^�C������.
	if ( m_DashTime			>= 0.f) { m_DashTime		-= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_ReloadTime		>= 0.f) { m_ReloadTime		-= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_DashCoolTime		>= 0.f) { m_DashCoolTime	-= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_BulletCoolTime	>= 0.f) { m_BulletCoolTime	-= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_SelectMoveTime	>= 0.f) { m_SelectMoveTime	-= CTime::GetInstance()->GetDeltaTime(); }

	// �s�����܂Ƃ߂��֐�.
	Act(chara);

	CCharacter::Update();
}


//=============================================================================
//		�`�揈��.
//=============================================================================
void CEnemy::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light)
{
	CCharacter::Draw( View, Proj, Light );

	// �e�̕`��.
	for (size_t i = 0; i < m_pBullets.size(); ++i) { m_pBullets[i]->Draw(View, Proj, Light); }

	// �e�̕`��.
	m_pGun->Draw(View, Proj, Light);
}


//=============================================================================
//		�����蔻�菈��.
//=============================================================================
void CEnemy::Collision(std::unique_ptr<CPlayer>& egg, Collider floor, Collider cylinder)
{
	Collider Bullet, enemyegg;

	// �G�t�F�N�g�ɕK�v�ȃn���h����p��.
	static ::EsHandle hEffect = -1;

	// �v���C���[�f�[�^���擾.
	enemyegg.SetVertex( egg->GetObjeInfo(), egg->GetMesh()->GetVertices());

	// �e�̔���.
	for (size_t i = 0; i < m_pBullets.size(); ++i) 
	{
		// �e�f�[�^���擾.
		Bullet.SetVertex( m_pBullets[i]->GetObjeInfo(), m_pMeshBullet->GetVertices());


		// �����蔻����p�̕ϐ���錾.
		CollisionPoints pointsbc, pointsbf, pointsbe;
		pointsbc = m_pGJK->GJK(Bullet, cylinder);
		pointsbf = m_pGJK->GJK(Bullet, floor);
		pointsbe = m_pGJK->GJK(Bullet, enemyegg);


		// ���⏰�ɂ��������ꍇ�폜.
		if (pointsbc.Col || pointsbf.Col) {
			// �G�t�F�N�g�̍Đ�.
			hEffect = CEffect::Play(CEffect::Dust, m_pBullets[i]->GetPos() - m_pBullets[i]->GetMoveVec() * 2);

			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}


		// �v���C���[�ɒe�����������ꍇ.
		if (pointsbe.Col) 
		{
			// �w�b�h�V���b�g����(�C������).
			if (m_pBullets[i]->GetPos().y < egg->GetPos().y + m_EggAirRoomY) {
				// HP���N���e�B�J���_���[�W�����炷.
				egg->CritDamage();
				CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_DamageCrit);
			}
			else {
				// HP�𓷑̃_���[�W�����炷.
				egg->BodyDamage();
				CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_Damage);
			}

			// ��������.
			m_Hit = true;

			// ������ɍ폜.
			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}
	}
}


//-----------------------------------------------------------------------------
//		�s�����܂Ƃߊ֐�.
//-----------------------------------------------------------------------------
void CEnemy::Act(std::unique_ptr<CPlayer>& chara)
{
	CRandom random;
	
	//-------------------------------------------------------------------------
	//		�����_���ňړ�.
	//-------------------------------------------------------------------------

	// ���̍s������܂ł̎��Ԃ� 0 �ȉ��̏ꍇ.
	if (m_SelectMoveTime <= 0.f) {
		m_MoveKind = random.GetRandomInt(0, MoveKind::max - 1);
		m_SelectMoveTime = m_SelectMoveTimeMax;
	}

	// �_�b�V�����͑���ł��Ȃ��悤�ɂ���.
	if (m_DashTime <= 0.f) {
		// ���삪�\�ȊԂ͏���������.
		DashVec = ZEROVEC3;

		// �J�����̌����x�N�g�����擾.
		D3DXVECTOR3 camDir = CCamera::GetInstance()->GetCamDir();
		camDir.y = 0.f;	// Y��񂪂���Ɣ�юn�߂�̂�Y�̗v�f�𔲂�.
		D3DXVec3Normalize(&camDir, &camDir); // ���K��.

		// �ړ���������x�N�g��.
		D3DXVECTOR3 forward(ZEROVEC3);
		D3DXVECTOR3 left(ZEROVEC3);
		D3DXVECTOR3 upvec(0, 1, 0);

		// ���x�N�g�������߂�.
		D3DXVec3Cross(&left, &camDir, &upvec);
		D3DXVec3Normalize(&left, &left);

		if (m_MoveKind == MoveKind::Straight)	{ forward += camDir; }
		if (m_MoveKind == MoveKind::Back)		{ forward -= camDir; }
		if (m_MoveKind == MoveKind::Left)		{ forward += left; }
		if (m_MoveKind == MoveKind::Right)		{ forward -= left; }
		if (m_MoveKind == MoveKind::Wait)		{ forward = ZEROVEC3; }

		// �ŏI�I�ȃx�N�g���ʂ𑬓x�ɂ������v�x�N�g���ɓn��.
		m_SumVec += forward * m_MoveSpeed;
	}


	//-------------------------------------------------------------------------
	//		�����_���Ń_�b�V��.
	//-------------------------------------------------------------------------

	// �N�[���^�C�����I�����Ă�����_�b�V���\��.
	if (m_DashCoolTime <= 0.f) { m_CanDash = true; }

	// �����_���Ƀ_�b�V���̃^�C�~���O�����߂�.
	bool dash = (random.GetRandomInt(0, 120) == 0);

	// �����������Ă���ꍇ.
	if (dash && m_CanDash) {
		// �_�b�V�����Ԃ̐ݒ�.
		m_DashTime = m_DashTimeMax;
		// �_�b�V���N�[���^�C���̐ݒ�.
		m_DashCoolTime = m_DashCoolTimeMax;

		// ���v�x�N�g���ɏ�񂪂Ȃ��ꍇ.
		if (m_SumVec == ZEROVEC3) {
			D3DXVECTOR3 camDir;
			// �����x�N�g�����X�V.
			camDir.x = cos(D3DXToRadian(m_vRotation.y)) * cos(D3DXToRadian(m_vRotation.x));
			camDir.y = sin(D3DXToRadian(m_vRotation.x));
			camDir.z = sin(D3DXToRadian(m_vRotation.y)) * cos(D3DXToRadian(m_vRotation.x));

			// �J�����̌����x�N�g�����擾.
			camDir.y = 0.f;	// Y��񂪂���Ɣ�юn�߂�̂�Y�̗v�f�𔲂�.
			D3DXVec3Normalize(&camDir, &camDir); // ���K��.
			DashVec = camDir * m_MoveSpeed * m_DashSpeed;
			m_CanDash = false;
		}
		else {
			DashVec = m_SumVec * m_DashSpeed;
			m_CanDash = false;
		}
	}


	//-------------------------------------------------------------------------
	//		�����_���W�����v����.
	//-------------------------------------------------------------------------

	// �����_���ɃW�����v�̃^�C�~���O�����߂�.
	bool jump = (random.GetRandomInt(0, 120) == 0);

	// �����_���ɃW�����v�̃^�C�~���O�����߂�.
	if (jump && m_CanJump) {
		m_JumpPower = m_JumpPowerMax;
		m_CanJump = false;
	}
	// �W�����v�͂�Y�l�ɉ��Z.
	m_vPosition.y += m_JumpPower;


	// �J�����̃��CHit���W���甭�˒n�_�̃x�N�g�����v�Z.
	D3DXVECTOR3 shootdir = chara->GetPos() - m_pGun->GetShootPos();
	shootdir.y += 0.2f; // �␳�l������.
	D3DXVec3Normalize(&shootdir, &shootdir);	// ���K��.


	//-------------------------------------------------------------------------
	//		�����_���Ŏˌ�.
	//-------------------------------------------------------------------------

	// �N�[���^�C�����I�����Ă�����ˌ��\.
	if (m_BulletCoolTime <= 0.f) { m_CanShot = true; }

	// �����_���Ɏˌ��^�C�~���O�����߂�.
	bool shot = (random.GetRandomInt(0, 4) == 0);

	// �ˌ������������Ă����ꍇ.
	if (shot && m_CanShot && m_CharaInfo.Ammo != 0 && m_ReloadTime <= 0) 
	{
		m_CanShot = false;						// �ˌ��s�ɂ���.
		m_CharaInfo.Ammo--;						// �c�e�������炷.
		m_BulletCoolTime = m_BulletCoolTimeMax;	// �Ĕ��˂܂ł̎��Ԃ�ݒ�.

		m_pBullets.push_back(std::make_unique<CBullet>());

		m_pBullets.back()->AttachMesh(*m_pMeshBullet);	// ���b�V����ݒ�.
		m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);	// null�ɂȂ�Ȃ��悤�Ɍ����Ȃ����W�ɏ����ݒ�.
		m_pBullets.back()->SetScale(5.f, 5.f, 5.f);		// �T�C�Y��ݒ�.

		// �e�̏����ʒu,�ړ������̒P�ʃx�N�g��,���x��ݒ�.
		m_pBullets.back()->Init(
			m_pGun->GetShootPos(),
			shootdir,
			m_BulletSpeed);
	}

	// �e�؂�łŃ����[�h.
	if (m_CharaInfo.Ammo == 0) {
		m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;	// �c�e���ɍő�e�򐔂�ݒ�.
		m_ReloadTime = m_ReloadTimeMax;			// �����[�h���Ԃ̐ݒ�.
	}

	// ���v�̃x�N�g���ʕ��ʒu���X�V.
	m_vPosition += m_SumVec + DashVec;
}