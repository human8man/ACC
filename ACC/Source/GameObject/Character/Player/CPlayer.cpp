#include "CPlayer.h"
#include "DirectSound/CSoundManager.h"
#include "Common/DirectInput/CDirectInput.h"
#include "Camera/CCamera.h"
#include "Character/Enemy/CEnemy.h"

#if _DEBUG
#include "ImGui/CImGui.h"
#endif

//============================================================================
//		�v���C���[�N���X.
//============================================================================
CPlayer::CPlayer()
	: m_pGJK		( nullptr )
	, m_TurnSpeed	( 0.1f )
	, m_MoveSpeed	( 0.2f )
	, m_CamRevision	( 2.f )
	, m_SumVec		( ZEROVEC3 )
{
	m_CharaInfo.HP = 8;
}

CPlayer::~CPlayer()
{
}


//============================================================================
//		�X�V����.
//============================================================================
void CPlayer::Update()
{
	// ���t���[�����Z�b�g����.
	m_SumVec = ZEROVEC3;

	// �J�����Ɍ��������킹��.
	m_vRotation.y = CCamera::GetInstance()->GetRot().y;
	
	// �_�b�V���֘A��0�ȏ�̂��̂�����ꍇ�J�E���g������.
	if ( m_DashCoolTime >= 0.f ) { m_DashCoolTime -= CTime::GetInstance()->GetDeltaTime(); }
	if ( m_DashTime >= 0.f ){ m_DashTime -= CTime::GetInstance()->GetDeltaTime(); }

	// ���͏���.
	KeyInput();

	// ���͏�����ɃJ�������W���Z�b�g.
	if (!CCamera::GetInstance()->GetMoveCamera()) {
		// �v���C���[�ʒu + �v���C���[�̍������܂񂾍��W��n��.
		D3DXVECTOR3 campos = m_vPosition;
		campos.y += m_CamRevision;
		 CCamera::GetInstance()->SetPosition(campos);
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
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}

		// �G�l�~�[�ƒe�����������ꍇ.
		if ( pointsbe.Col ) {
			// �w�b�h�V���b�g����(�C������).
			if (m_pBullets[i]->GetPos().y < egg->GetPos().y + m_EggAirRoomY) { egg->DubleDecreHP(); }
			else  { egg->DecreHP(); }

			// �����������Ƃ͍폜.
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
			continue;
		}
	}
}


//-----------------------------------------------------------------------------
//		�L�[���͏���.
//-----------------------------------------------------------------------------
void CPlayer::KeyInput()
{
	CKey*	Key		= CDInput::GetInstance()->CDKeyboard();
	CMouse* Mouse	= CDInput::GetInstance()->CDMouse();
	

	//----------------------------------------------------------------------
	//		WASD�ňړ�.
	//----------------------------------------------------------------------
	// �_�b�V�����͑���ł��Ȃ��悤�ɂ���.
	if ( m_DashTime <= 0.f ) {
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
	if ( m_DashCoolTime <= 0.f ) { m_CanDash = true; }

	// SHIFT�Ń_�b�V��.
	if ( Key->IsKeyAction(DIK_LSHIFT) && m_CanDash) {
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
	}

	//----------------------------------------------------------------------
	//		�W�����v����.
	//----------------------------------------------------------------------
	if ( Key->IsKeyAction(DIK_SPACE) && m_CanJump ) {
		m_JumpPower = m_JumpPowerMax; 
		m_CanJump = false; 
	}
	// �W�����v�͂�Y�l�ɉ��Z.
	m_vPosition.y += m_JumpPower;


	// �J�����̃��CHit���W���甭�˒n�_�̃x�N�g�����v�Z.
	D3DXVECTOR3 shootdir = CCamera::GetInstance()->GetRayHit() - m_pGun->GetShootPos();
	D3DXVec3Normalize(&shootdir, &shootdir);	// ���K��.

	//----------------------------------------------------------------------
	//		���N���b�N�Ŏˌ�.
	//----------------------------------------------------------------------
	if (Mouse->IsLAction()) {
		m_pBullets.push_back(new CBullet());

		m_pBullets.back()->AttachMesh(*m_pMeshBullet);	// ���b�V����ݒ�.
		m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);	// null�ɂȂ�Ȃ��悤�Ɍ����Ȃ����W�ɏ����ݒ�.
		m_pBullets.back()->SetScale(5.f, 5.f, 5.f);	// �T�C�Y��ݒ�.


		// �e�̏����ʒu,�ړ������̒P�ʃx�N�g��,���x��ݒ�.
		m_pBullets.back()->Init(
			m_pGun->GetShootPos(),
			shootdir,
			0.8f );
	}

	// ���v�̃x�N�g���ʕ��ʒu���X�V.
	m_vPosition += m_SumVec + DashVec;

#if _DEBUG
	ImGui::Begin("DashWindow");
	ImGui::Text("DashCoolTime%f", m_DashCoolTime);
	ImGui::Text("DashTime%f", m_DashTime);
	ImGui::Text("DeltaTime%f", CTime::GetInstance()->GetDeltaTime() );
	ImGui::Text("%f,%f,%f", DashVec.x, DashVec.y, DashVec.z);
	ImGui::End();

#endif
}