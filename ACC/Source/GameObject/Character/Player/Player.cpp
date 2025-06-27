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
	// �L�����N�^�[CSV�̃p�X
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
	// ������
	m_CharaInfo.HP = m_CharaInfo.MaxHP;
	m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;

	// �L�����N�^�[CSV�̏��ۑ��p
	std::unordered_map<std::string, std::string> m_StateList;
	// �L�����N�^�[CSV�̏��擾
	m_StateList = FileManager::CSVLoad(CharaCSVPath);
	
	// ��łȂ��ꍇ�́A�O���Œ�������ׂ��ϐ��̒l�����Ă���
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
//		�X�V����
//============================================================================
void Player::Update(std::unique_ptr<Enemy>& chara)
{
	// ���t���[�����Z�b�g����
	m_SumVec = ZEROVEC3;
	m_Hit = false;

	// �J�����Ɍ��������킹��
	m_vRotation.y = Camera::GetInstance()->GetRot().y;
	
	// 0�ȏ�̂��̂�����ꍇ�J�E���g������
	if ( m_DashTime >= 0.f )	{ m_DashTime -= Time::GetDeltaTime(); }
	if ( m_ReloadTime >= 0.f )	{
		m_ReloadTime -= Time::GetDeltaTime(); 
		// �����[�h�I������SE��炷
		if (m_ReloadTime < 0.f) {
			SoundManager::GetInstance()->Play(SoundManager::enList::SE_ReloadEnd);
		}
	}
	if ( m_DashCoolTime >= 0.f )	{ m_DashCoolTime	-= Time::GetDeltaTime(); }
	if ( m_BulletCoolTime >= 0.f )	{ m_BulletCoolTime	-= Time::GetDeltaTime(); }

	// ���͏���
	KeyInput(chara);

	// ���͏�����ɃJ�������W���Z�b�g
	if (!Camera::GetInstance()->GetMoveCamera()) {
		// �v���C���[�ʒu + �v���C���[�̍������܂񂾍��W��n��
		D3DXVECTOR3 campos = m_vPosition;
		campos.y += m_CamRevision;
		Camera::GetInstance()->SetPosition(campos);
	}

	// �z�[�~���O�̃`�[�g���L���ȏꍇ
	if (m_Homing) {
		for (size_t i = 0; i < m_pBullets.size(); ++i) 
		{
			D3DXVECTOR3 shootdir, EnemyPos;

			// �G���W���擾(���_���ƒn�ʂɓ����邽�ߏ���������)
			EnemyPos = chara->GetPos();
			EnemyPos.y += 0.5f;

			// �e���W����G���W�̃x�N�g�����o��
			shootdir = EnemyPos - m_pBullets[i]->GetPos();
			D3DXVec3Normalize(&shootdir, &shootdir); // ���K��

			// �e�̏����ʒu,�ړ������̒P�ʃx�N�g��,���x��ݒ�
			m_pBullets[i]->Init( m_pBullets[i]->GetPos(), shootdir, m_BulletSpeed);
		}
	}

	// �L�����N�^�[�̍X�V����
	Character::Update();

	// ImGui��p�����f�o�b�O�֐�
	if (ISDEBUG) { PlayerImGui(); }
}


//============================================================================
//		�`�揈��
//============================================================================
void Player::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	// �v���C���[�͕`�悵�Ȃ�
	// Character::Draw( View, Proj, Light );

	// �G�t�F�N�g���ɕK�v�ȃn���h����p��
	static ::EsHandle hEffect = -1;

	D3DXVECTOR4 color = { 0.f, 0.f, 1.f, 1.f };

	// �e�Ƃ��̃G�t�F�N�g�̕`��
	for (size_t i = 0; i < m_pBullets.size(); ++i) { 
		hEffect = Effect::Play(Effect::BulletSmoke, m_pBullets[i]->GetPos());
		m_pBullets[i]->Draw(View, Proj, Light);

		// �E�H�[���n�b�N���̓��b�V�������o��
		if (m_WallHack) {
			DirectX11::GetInstance()->SetDepth(false);
			m_pMeshLine->DrawMeshWireframeFromVertices(*m_pBullets[i]->GetMesh(), *m_pBullets[i], View, Proj, color, 5.f);
			m_pMeshLine->Render(View, Proj);
			DirectX11::GetInstance()->SetDepth(true);
		}
	}

	// �e�̕`��
	m_pGun->Draw(View, Proj, Light);
}


//============================================================================
//		�����蔻�菈��
//============================================================================
void Player::Collision(std::unique_ptr<Enemy>& egg, Collider floor, Collider cylinder)
{
	// �����`�F�b�N�p�ϐ�
	float DistanceToFloorY		= 5.f;		// ���Ƃ�Y���W����
	float DistanceToCylinder	= 100.f;	// ��
	float DistanceToEnemy		= 30.f;		// �G
	Collider Bullet,enemyegg;

	// �G�t�F�N�g���ɕK�v�ȃn���h����p��
	static ::EsHandle hEffect = -1;

	// �G�f�[�^���擾
	enemyegg.SetVertex( egg->GetObjectInfo(), egg->GetMesh()->GetVertices());

	// �e�̔���
	for (size_t i = 0; i < m_pBullets.size(); ++i) {
		// �e���S�_�̎擾
		D3DXVECTOR3 bulletCenter   = m_pBullets[i]->GetPos();
		D3DXVECTOR3 floorCenter    = floor.GetCenter();
		D3DXVECTOR3 cylinderCenter = cylinder.GetCenter();
		D3DXVECTOR3 enemyCenter    = enemyegg.GetCenter();
		
		// �����v�Z
		D3DXVECTOR3 diffCylinder = bulletCenter - cylinderCenter;
		D3DXVECTOR3 diffEnemy    = bulletCenter - enemyCenter;
		
		float diffY				= fabsf(bulletCenter.y - floorCenter.y);
		float distToCylinder	= D3DXVec3Length(&diffCylinder);
		float distToEnemy		= D3DXVec3Length(&diffEnemy);

		// �e�f�[�^���擾
		Bullet.SetVertex( m_pBullets[i]->GetObjectInfo(), m_pMeshBullet->GetVertices());
		
		// �����蔻����p�̕ϐ���錾
		CollisionPoints pointsbc, pointsbf, pointsbe;

		// �������߂��Ƃ���������
		if (diffY < DistanceToFloorY)
			pointsbf = m_pGJK->GJKC(Bullet, floor);

		if (distToCylinder < DistanceToCylinder)
			pointsbc = m_pGJK->GJKC(Bullet, cylinder);

		if (distToEnemy < DistanceToEnemy)
			pointsbe = m_pGJK->GJKC(Bullet, enemyegg);

		// ���⏰�ɂ��������ꍇ�폜
		if (pointsbc.Col || pointsbf.Col)
		{
			// �G�t�F�N�g�̍Đ�
			hEffect = Effect::Play(Effect::Dust, m_pBullets[i]->GetPos() - m_pBullets[i]->GetMoveVec() * 2);

			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}

		// �G�l�~�[�ƒe�����������ꍇ
		if ( pointsbe.Col ) {
			// �w�b�h�V���b�g����(�C������)
			if (m_pBullets[i]->GetPos().y < egg->GetPos().y + m_EggAirRoomY) {
				// HP���N���e�B�J���_���[�W�����炷
				egg->CritDamage();
				// �G�t�F�N�g�̍Đ�
				hEffect = Effect::Play(Effect::CritHit, egg->GetPos());
				// ������ނ̐ݒ�
				m_HitKind = HitKind::Crit;
				// �N���e�B�J����������炷
				SoundManager::GetInstance()->Play(SoundManager::enList::SE_CritHit);
			}
			else {
				// HP�𓷑̃_���[�W�����炷
				egg->BodyDamage();
				// �G�t�F�N�g���Y���Ă����̂ł��炵�Ă���G�t�F�N�g�̍Đ�
				D3DXVECTOR3 enemypos = egg->GetPos();
				enemypos.y += 2.f;
				hEffect = Effect::Play(Effect::Hit, enemypos);
				// ������ނ̐ݒ�
				m_HitKind = HitKind::Hit;
				// ��������炷
				SoundManager::GetInstance()->Play(SoundManager::enList::SE_Hit);
			}

			// ��������
			m_Hit = true;

			// �����������Ƃ͍폜
			m_pBullets[i].reset();
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}
	}
}


//-----------------------------------------------------------------------------
//		�L�[���͏���
//-----------------------------------------------------------------------------
void Player::KeyInput(std::unique_ptr<Enemy>& chara)
{

	Key* Key = DirectInput::GetInstance()->CDKeyboard();

	// �ړ�
	MoveProcess(chara);

	// �_�b�V��
	DashProcess();

	//----------------------------------------------------------------------
	//		�W�����v����
	//----------------------------------------------------------------------
	if (m_Landing) { m_JumpPower = 0; }
	if (Key->IsKeyAction(DIK_SPACE) && m_CanJump && m_Landing) {
		m_JumpPower = m_JumpPowerMax;
		m_CanJump = m_Landing = false;
		// �������ɂ�蒅�n���肪�o�Ă��܂����߁A�������ɏ����グ��
		m_vPosition.y += m_JumpPower * Time::GetDeltaTime();
		SoundManager::GetInstance()->Play(SoundManager::enList::SE_Jump);
	}

	// �ˌ�
	ShotProcess();

	//----------------------------------------------------------------------
	//		R�Ń����[�h
	//----------------------------------------------------------------------
	if (Key->IsKeyAction(DIK_R)) {
		m_CharaInfo.Ammo = m_CharaInfo.MaxAmmo;
		m_ReloadTime = m_ReloadTimeMax;
		SoundManager::GetInstance()->Play(SoundManager::enList::SE_Reload);
	}

	//----------------------------------------------------------------------
	//		�`�[�g�֘A
	//----------------------------------------------------------------------
	if (Key->IsKeyAction(DIK_1)) { m_AutoAim = !m_AutoAim; }
	if (Key->IsKeyAction(DIK_2)) { m_Homing = !m_Homing; }
	if (Key->IsKeyAction(DIK_3)) { m_WallHack = !m_WallHack; }
	if (Key->IsKeyAction(DIK_4)) { m_TriggerHappy = !m_TriggerHappy; RAINBOW_WINDOW = m_TriggerHappy; }


	// ���v�̃x�N�g���ʕ��ʒu���X�V
	m_vPosition += (m_SumVec + m_DashVec) * Time::GetDeltaTime();
}


//-----------------------------------------------------------------------------
//		�ړ�����
//-----------------------------------------------------------------------------
void Player::MoveProcess(std::unique_ptr<Enemy>& chara)
{
	// �_�b�V�����͑���ł��Ȃ��悤�ɂ���
	if (0.f < m_DashTime) { return; }

	Key* Key = DirectInput::GetInstance()->CDKeyboard();

	// ���삪�\�ȊԂ͏���������
	m_DashVec = ZEROVEC3;

	D3DXVECTOR3 camDir;
	// �J�����̌����x�N�g�����擾
	if (m_AutoAim) {
		camDir = chara->GetPos() - m_vPosition;
		camDir.y = 0.f;	// Y��񂪂���Ɣ�юn�߂�̂�Y�̗v�f�𔲂�
		D3DXVec3Normalize(&camDir, &camDir); // ���K��
	}
	else {
		camDir = Camera::GetInstance()->GetCamDir();
		camDir.y = 0.f;	// Y��񂪂���Ɣ�юn�߂�̂�Y�̗v�f�𔲂�
		D3DXVec3Normalize(&camDir, &camDir); // ���K��
	}

	// �ړ���������x�N�g��
	D3DXVECTOR3 forward(ZEROVEC3);
	D3DXVECTOR3 left(ZEROVEC3);
	D3DXVECTOR3 upvec(0, 1, 0);

	// ���x�N�g�������߂�
	D3DXVec3Cross(&left, &camDir, &upvec);
	D3DXVec3Normalize(&left, &left);

	if ( Key->IsKeyDown(DIK_W)) { forward += camDir; }
	if ( Key->IsKeyDown(DIK_S)) { forward -= camDir; }
	if ( Key->IsKeyDown(DIK_A)) { forward += left; }
	if ( Key->IsKeyDown(DIK_D)) { forward -= left; }

	// �ŏI�I�ȃx�N�g���ʂ𑬓x�ɂ������v�x�N�g���ɓn��
	m_SumVec += forward * m_MoveSpeed;
}


//-----------------------------------------------------------------------------
//		�_�b�V������
//-----------------------------------------------------------------------------
void Player::DashProcess()
{
	Key* Key = DirectInput::GetInstance()->CDKeyboard();

	// �N�[���^�C�����I�����Ă�����_�b�V���\��
	if (m_DashCoolTime <= 0.f) { m_CanDash = true; }

	// SHIFT�Ń_�b�V��
	if (!(Key->IsKeyAction(DIK_LSHIFT) && m_CanDash && m_Landing)) { return; }

	// �_�b�V�����Ԃ̐ݒ�
	m_DashTime = m_DashTimeMax;
	// �_�b�V���N�[���^�C���̐ݒ�
	m_DashCoolTime = m_DashCoolTimeMax;

	// ���v�x�N�g���ɏ�񂪂Ȃ��ꍇ
	if (m_SumVec == ZEROVEC3) {
		// �J�����̌����x�N�g�����擾
		D3DXVECTOR3 camDir = Camera::GetInstance()->GetCamDir();
		camDir.y = 0.f;	// Y��񂪂���Ɣ�юn�߂�̂�Y�̗v�f�𔲂�
		D3DXVec3Normalize(&camDir, &camDir); // ���K��

		// �J�������� �~ �ړ����x �~ �_�b�V���{���̃x�N�g�����o��
		m_DashVec = camDir * m_MoveSpeed * m_DashSpeed;
		m_CanDash = false;
	}
	else {
		// ���v�x�N�g���Ƀ_�b�V���{����ς����l���o��
		m_DashVec = m_SumVec * m_DashSpeed;
		m_CanDash = false;
	}
	SoundManager::GetInstance()->Play(SoundManager::enList::SE_Dash);
}


//-----------------------------------------------------------------------------
//		���ˏ���
//-----------------------------------------------------------------------------
void Player::ShotProcess()
{
	Mouse* Mouse = DirectInput::GetInstance()->CDMouse();
	
	// �N�[���^�C�����I�����Ă�����ˌ��\
	if (m_BulletCoolTime <= 0.f) { m_CanShot = true; }

	// ���N���b�N�������ꂢ�Ȃ��ꍇ
	if (!(Mouse->IsLAction() || m_TriggerHappy && Mouse->IsLDown())) { return; }

	// �G�t�F�N�g���ɕK�v�ȃn���h����p��
	static ::EsHandle hEffect = -1;

	// �J�����̃��CHit���W���甭�˒n�_�̃x�N�g�����v�Z
	D3DXVECTOR3 shootdir = Camera::GetInstance()->GetRayHit() - m_pGun->GetShootPos();
	D3DXVec3Normalize(&shootdir, &shootdir);	// ���K��
	
	// �ˌ������������Ă���ꍇ
	if (m_CanShot && m_CharaInfo.Ammo != 0 && m_ReloadTime <= 0 || m_CanShot && m_TriggerHappy)
	{
		// �A�˃��[�h�̏ꍇ�N�[���^�C����c�e���̐ݒ�.
		if (m_TriggerHappy) {
			m_CanShot = false;
			m_BulletCoolTime = m_BulletCoolTimeMin;
		}
		else {
			m_CanShot = false;
			m_CharaInfo.Ammo--;
			m_BulletCoolTime = m_BulletCoolTimeMax;
		}

		// �e���쐬����
		m_pBullets.push_back(std::make_unique<Bullet>());

		m_pBullets.back()->AttachMesh(*m_pMeshBullet);	// ���b�V����ݒ�
		m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);	// null�ɂȂ�Ȃ��悤�Ɍ����Ȃ����W�ɏ����ݒ�
		m_pBullets.back()->SetScale(5.f, 5.f, 5.f);		// �T�C�Y��ݒ�

		// �e�̏����ʒu,�ړ������̒P�ʃx�N�g��,���x��ݒ�
		m_pBullets.back()->Init(
			m_pGun->GetShootPos(),
			shootdir,
			m_BulletSpeed);

		hEffect = Effect::Play(Effect::GunFire, m_pGun->GetShootPos());

		// �ˌ�����炷
		SoundManager::GetInstance()->Play(SoundManager::enList::SE_Shot);
	}
	else {
		// ��ł�����炷
		SoundManager::GetInstance()->Play(SoundManager::enList::SE_NoAmmo);
	}
}


//-----------------------------------------------------------------------------
//		�f�o�b�O���Ɏg�p����֐�
//-----------------------------------------------------------------------------
void Player::PlayerImGui()
{
	ImGui::Begin(IMGUI_JP("�v���C���[���"));
	ImGui::Text("Dash%f", m_DashTime);
	ImGui::Text("Reload%f", m_ReloadTime);
	ImGui::Text("DashCool%f", m_DashCoolTime);
	ImGui::Text("BulletCool%f", m_BulletCoolTime);
	ImGui::End();
}
