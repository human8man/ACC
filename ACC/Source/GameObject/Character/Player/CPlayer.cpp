#include "CPlayer.h"
#include "DirectSound/CSoundManager.h"
#include "Common/DirectInput/CDirectInput.h"
#include "Camera/CCamera.h"
#include "Character/Enemy/CEnemy.h"

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

	// �e�̔���.
	for (size_t i = 0; i < m_pBullets.size(); ++i) {

		Bullet.SetVertex(
			m_pBullets[i]->GetPos(),
			m_pBullets[i]->GetRot(),
			m_pBullets[i]->GetScale(),
			m_pMeshBullet->GetVertices());

		enemyegg.SetVertex(
			egg->GetPos(),
			egg->GetRot(),
			egg->GetScale(),
			egg->GetMesh()->GetVertices());

		CollisionPoints pointsbc = m_pGJK->GJK(Bullet, cylinder);
		CollisionPoints pointsbf = m_pGJK->GJK(Bullet, floor);
		CollisionPoints pointsbe = m_pGJK->GJK(Bullet, enemyegg);

		// ���⏰�ɂ��������ꍇ�폜.
		if (pointsbc.Col || pointsbf.Col) {
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
		}

		if ( pointsbe.Col ) {
			// �w�b�h�V���b�g����(�C������).
			if (m_pBullets[i]->GetPos().y < egg->GetPos().y + m_EggAirRoomY) { egg->DubleDecreHP(); }
			else  { egg->DecreHP(); }

			// �����������Ƃ͍폜.
			SAFE_DELETE(m_pBullets[i]);
			m_pBullets.erase(m_pBullets.begin() + i);
			--i;
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

	if (Key->IsKeyDown( DIK_W )) { forward += camDir ; }
	if (Key->IsKeyDown( DIK_S )) { forward -= camDir ; }
	if (Key->IsKeyDown( DIK_A )) { forward += left ; }
	if (Key->IsKeyDown( DIK_D )) { forward -= left ; }

	// �ŏI�I�ȃx�N�g���ʂ𑬓x�ɂ������v�x�N�g���ɓn��.
	m_SumVec += forward * m_MoveSpeed;

	//----------------------------------------------------------------------
	//		�W�����v����.
	//----------------------------------------------------------------------
	if ( Key->IsKeyAction(DIK_SPACE) && m_CanJump ) {
		m_JumpPower = m_JumpPowerMax; 
		m_CanJump = false; 
	}
	// �W�����v�͂�Y�l�ɉ��Z.
	m_vPosition.y += m_JumpPower;


	//----------------------------------------------------------------------
	//		���N���b�N�Ŏˌ�.
	//----------------------------------------------------------------------
	if (Mouse->IsLAction()) {
		m_pBullets.push_back(new CBullet());

		m_pBullets.back()->AttachMesh(*m_pMeshBullet);	// ���b�V����ݒ�.
		m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);	// null�ɂȂ�Ȃ��悤�Ɍ����Ȃ����W�ɏ����ݒ�.
		m_pBullets.back()->SetScale(10.f, 10.f, 10.f);	// �T�C�Y��ݒ�.




		// �e�̏����ʒu,�ړ������̒P�ʃx�N�g��,���x��ݒ�.
		m_pBullets.back()->Init(
			m_pGun->GetShootPos(),
			CCamera::GetInstance()->GetCamDir(),
			1.f );
	}

	// ���v�̃x�N�g���ʕ��ʒu���X�V.
	m_vPosition += m_SumVec;
}