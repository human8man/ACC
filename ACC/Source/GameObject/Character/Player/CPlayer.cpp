#include "CPlayer.h"
#include "DirectSound/CSoundManager.h"
#include "Common/DirectInput/CDirectInput.h"
#include "Camera/CCamera.h"
#include "Character/Enemy/CEnemy.h"
#include "Effect/CEffect.h"


//============================================================================
//		�v���C���[�N���X.
//============================================================================
CPlayer::CPlayer()
	: m_pGJK		( nullptr )
	, m_TurnSpeed	( 0.1f )
	, m_MoveSpeed	( 0.2f )
	, m_CamRevision	( 4.f )
	, m_SumVec		( ZEROVEC3 )
	, m_AutoAim		(false)
	, m_Homing		(false)
{
	m_CharaInfo.HP = m_CharaInfo.MaxHP;
	m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;
}

CPlayer::~CPlayer()
{
}


//============================================================================
//		�X�V����.
//============================================================================
void CPlayer::Update(std::unique_ptr<CEnemy>& chara)
{
	// ���t���[�����Z�b�g����.
	m_SumVec = ZEROVEC3;
	m_Hit = false;

	// �J�����Ɍ��������킹��.
	m_vRotation.y = CCamera::GetInstance()->GetRot().y;
	
	// 0�ȏ�̂��̂�����ꍇ�J�E���g������.
	if ( m_DashTime >= 0.f )	{ m_DashTime -= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_ReloadTime >= 0.f )	{
		m_ReloadTime -= CTime::GetInstance()->GetDeltaTime(); 
		if (m_ReloadTime < 0.f) {
			// �����[�h�I��.
			CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_ReloadEnd);
		}
	}
	if ( m_DashCoolTime >= 0.f )	{ m_DashCoolTime	-= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_BulletCoolTime >= 0.f )	{ m_BulletCoolTime	-= CTime::GetInstance()->GetDeltaTime(); }

	// ���͏���.
	KeyInput(chara);

	// ���͏�����ɃJ�������W���Z�b�g.
	if (!CCamera::GetInstance()->GetMoveCamera()) {
		// �v���C���[�ʒu + �v���C���[�̍������܂񂾍��W��n��.
		D3DXVECTOR3 campos = m_vPosition;
		campos.y += m_CamRevision;
		CCamera::GetInstance()->SetPosition(campos);
	}

	// �z�[�~���O�̃`�[�g���L���ȏꍇ.
	if (m_Homing) {
		for (size_t i = 0; i < m_pBullets.size(); ++i) {
			D3DXVECTOR3 shootdir, EnemyPos;
			EnemyPos = chara->GetPos();
			EnemyPos.y += 0.5f;
			shootdir = EnemyPos - m_pBullets[i]->GetPos();
			D3DXVec3Normalize(&shootdir, &shootdir);	// ���K��.

			// �e�̏����ʒu,�ړ������̒P�ʃx�N�g��,���x��ݒ�.
			m_pBullets[i]->Init(
				m_pBullets[i]->GetPos(),
				shootdir,
				m_BulletSpeed);
		}
	}

	CCharacter::Update();
}


//============================================================================
//		�`�揈��.
//============================================================================
void CPlayer::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	// �v���C���[�͕`�悵�Ȃ�.
	// CCharacter::Draw( View, Proj, Light );
	
	// �e�̕`��.
	for (size_t i = 0; i < m_pBullets.size(); ++i) { m_pBullets[i]->Draw(View, Proj, Light); }

	// �e�̕`��.
	m_pGun->Draw(View, Proj, Light);
}


//============================================================================
//		�����蔻�菈��.
//============================================================================
void CPlayer::Collision(std::unique_ptr<CEnemy>& egg, MeshCollider floor, MeshCollider cylinder)
{
	MeshCollider Bullet,enemyegg;

	/// �G�t�F�N�g���ɕK�v�ȃn���h����p��.
	static ::EsHandle hEffect = -1;

	// �G�f�[�^���擾.
	enemyegg.SetVertex(
		egg->GetPos(),
		egg->GetRot(),
		egg->GetScale(),
		egg->GetMesh()->GetVertices());

	// �e�̔���.
	for (size_t i = 0; i < m_pBullets.size(); ++i) {

		// �e�f�[�^���擾.
		Bullet.SetVertex(
			m_pBullets[i]->GetPos(),
			m_pBullets[i]->GetRot(),
			m_pBullets[i]->GetScale(),
			m_pMeshBullet->GetVertices());


		// �����蔻����p�̕ϐ���錾.
		CollisionPoints pointsbc, pointsbf, pointsbe;
		pointsbc = m_pGJK->GJK(Bullet, cylinder);
		pointsbf = m_pGJK->GJK(Bullet, floor);
		pointsbe = m_pGJK->GJK(Bullet, enemyegg);


		// ���⏰�ɂ��������ꍇ�폜.
		if (pointsbc.Col || pointsbf.Col) {
			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}


		// �G�l�~�[�ƒe�����������ꍇ.
		if ( pointsbe.Col ) {
			// �w�b�h�V���b�g����(�C������).
			if (m_pBullets[i]->GetPos().y < egg->GetPos().y + m_EggAirRoomY) 
			{ 
				// HP��3�{���炷.
				egg->TripleDecreHP();
				// �G�t�F�N�g�̍Đ�.
				hEffect = CEffect::Play(CEffect::BodyHitCrit, egg->GetPos());
				// ������ނ̐ݒ�.
				m_HitKind = HitKind::Crit;
				// �N���e�B�J����������炷.
				CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_CritHit);
			}
			else  {
				// HP�����炷.
				egg->DecreHP();
				// �G�t�F�N�g���Y���Ă����̂ł��炵�Ă���G�t�F�N�g�̍Đ�.
				D3DXVECTOR3 enemypos = egg->GetPos();
				enemypos.y += 2.f;
				hEffect = CEffect::Play(CEffect::ShieldHit, enemypos);
				// ������ނ̐ݒ�.
				m_HitKind = HitKind::Hit;
				// ��������炷.
				CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_Hit);
			}

			// ��������.
			m_Hit = true;

			// �����������Ƃ͍폜.
			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}
	}
}


//-----------------------------------------------------------------------------
//		�L�[���͏���.
//-----------------------------------------------------------------------------
void CPlayer::KeyInput(std::unique_ptr<CEnemy>& chara)
{
	CKey* Key = CDInput::GetInstance()->CDKeyboard();
	CMouse* Mouse = CDInput::GetInstance()->CDMouse();


	//----------------------------------------------------------------------
	//		WASD�ňړ�.
	//----------------------------------------------------------------------

	// �_�b�V�����͑���ł��Ȃ��悤�ɂ���.
	if (m_DashTime <= 0.f) {
		// ���삪�\�ȊԂ͏���������.
		DashVec = ZEROVEC3;

		D3DXVECTOR3 camDir;
		// �J�����̌����x�N�g�����擾.
		if (m_AutoAim) {
			camDir = chara->GetPos() - m_vPosition;
			camDir.y = 0.f;	// Y��񂪂���Ɣ�юn�߂�̂�Y�̗v�f�𔲂�.
			D3DXVec3Normalize(&camDir, &camDir); // ���K��.
		}
		else {
			camDir = CCamera::GetInstance()->GetCamDir();
			camDir.y = 0.f;	// Y��񂪂���Ɣ�юn�߂�̂�Y�̗v�f�𔲂�.
			D3DXVec3Normalize(&camDir, &camDir); // ���K��.
		}

		// �ړ���������x�N�g��.
		D3DXVECTOR3 forward(ZEROVEC3);
		D3DXVECTOR3 left(ZEROVEC3);
		D3DXVECTOR3 upvec(0, 1, 0);

		// ���x�N�g�������߂�.
		D3DXVec3Cross(&left, &camDir, &upvec);
		D3DXVec3Normalize(&left, &left);

		if (Key->IsKeyDown(DIK_W)) { forward += camDir; }
		if (Key->IsKeyDown(DIK_S)) { forward -= camDir; }
		if (Key->IsKeyDown(DIK_A)) { forward += left; }
		if (Key->IsKeyDown(DIK_D)) { forward -= left; }

		// �ŏI�I�ȃx�N�g���ʂ𑬓x�ɂ������v�x�N�g���ɓn��.
		m_SumVec += forward * m_MoveSpeed;
	}

	//----------------------------------------------------------------------
	//		SHIFT�Ń_�b�V��.
	//----------------------------------------------------------------------

	// �N�[���^�C�����I�����Ă�����_�b�V���\��.
	if (m_DashCoolTime <= 0.f) { m_CanDash = true; }

	// SHIFT�Ń_�b�V��.
	if (Key->IsKeyAction(DIK_LSHIFT) && m_CanDash) {
		// �_�b�V�����Ԃ̐ݒ�.
		m_DashTime = m_DashTimeMax;
		// �_�b�V���N�[���^�C���̐ݒ�.
		m_DashCoolTime = m_DashCoolTimeMax;

		// ���v�x�N�g���ɏ�񂪂Ȃ��ꍇ.
		if (m_SumVec == ZEROVEC3) {
			// �J�����̌����x�N�g�����擾.
			D3DXVECTOR3 camDir = CCamera::GetInstance()->GetCamDir();
			camDir.y = 0.f;	// Y��񂪂���Ɣ�юn�߂�̂�Y�̗v�f�𔲂�.
			D3DXVec3Normalize(&camDir, &camDir); // ���K��.
			DashVec = camDir * m_MoveSpeed * m_DashSpeed;
			m_CanDash = false;
		}
		else {
			DashVec = m_SumVec * m_DashSpeed;
			m_CanDash = false;
		}
		CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_Dash);
	}

	//----------------------------------------------------------------------
	//		�W�����v����.
	//----------------------------------------------------------------------
	if (Key->IsKeyAction(DIK_SPACE) && m_CanJump) {
		m_JumpPower = m_JumpPowerMax;
		m_CanJump = false;
		CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_Jump);
	}
	// �W�����v�͂�Y�l�ɉ��Z.
	m_vPosition.y += m_JumpPower;


	// �J�����̃��CHit���W���甭�˒n�_�̃x�N�g�����v�Z.
	D3DXVECTOR3 shootdir = CCamera::GetInstance()->GetRayHit() - m_pGun->GetShootPos();
	D3DXVec3Normalize(&shootdir, &shootdir);	// ���K��.

	//----------------------------------------------------------------------
	//		���N���b�N�Ŏˌ�.
	//----------------------------------------------------------------------

	// �N�[���^�C�����I�����Ă�����ˌ��\.
	if (m_BulletCoolTime <= 0.f) { m_CanShot = true; }

	if (Mouse->IsLAction()){
		if (m_CanShot && m_CharaInfo.Ammo != 0 && m_ReloadTime <= 0) {
			m_CanShot = false;
			m_CharaInfo.Ammo--;
			m_BulletCoolTime = m_BulletCoolTimeMax;

			m_pBullets.push_back(std::make_unique<CBullet>());

			m_pBullets.back()->AttachMesh(*m_pMeshBullet);	// ���b�V����ݒ�.
			m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);	// null�ɂȂ�Ȃ��悤�Ɍ����Ȃ����W�ɏ����ݒ�.
			m_pBullets.back()->SetScale(5.f, 5.f, 5.f);		// �T�C�Y��ݒ�.

			// �e�̏����ʒu,�ړ������̒P�ʃx�N�g��,���x��ݒ�.
			m_pBullets.back()->Init(
				m_pGun->GetShootPos(),
				shootdir,
				m_BulletSpeed);

			// �ˌ�����炷.
			CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_Shot);
		}
		else if (m_CanShot && m_CharaInfo.Ammo == 0 && m_ReloadTime <= 0) {
			// ��ł�����炷.
			CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_NoAmmo);
		}
	}

	//----------------------------------------------------------------------
	//		R�Ń����[�h.
	//----------------------------------------------------------------------
	if (Key->IsKeyAction(DIK_R)) {
		m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;
		m_ReloadTime = m_ReloadTimeMax;
		CSoundManager::GetInstance()->PlaySE(CSoundManager::enList::SE_Reload);
	}

	//----------------------------------------------------------------------
	//		�`�[�g�֘A.
	//----------------------------------------------------------------------
	if (Key->IsKeyAction(DIK_1)) { m_AutoAim = !m_AutoAim; }
	if (Key->IsKeyAction(DIK_2)) { m_Homing = !m_Homing; }


	// ���v�̃x�N�g���ʕ��ʒu���X�V.
	m_vPosition += m_SumVec + DashVec;
}