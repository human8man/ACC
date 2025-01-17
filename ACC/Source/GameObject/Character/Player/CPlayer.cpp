#include "CPlayer.h"
#include "DirectSound/CSoundManager.h"
#include "Common/DirectInput/CDirectInput.h"
#include "Camera/CCamera.h"

//============================================================================
//		�v���C���[�N���X.
//============================================================================
CPlayer::CPlayer()
	: m_TurnSpeed	( 0.1f )
	, m_MoveSpeed	( 0.1f )
	, m_CamRevision	( 2.f )
	, m_JumpPower	( 117.6f )
	, m_SumVec		(ZEROVEC3)
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


//-----------------------------------------------------------------------------
//		�L�[���͏���.
//-----------------------------------------------------------------------------
void CPlayer::KeyInput()
{
	CKey* Key = CDInput::GetInstance()->CDKeyboard();
	CMouse* Mouse = CDInput::GetInstance()->CDMouse();
	

	//----------------------------------------------------------------------
	//		�ړ�����.
	//----------------------------------------------------------------------
	
	// �J�����̌����x�N�g�����擾.
	D3DXVECTOR3 camDir = CCamera::GetInstance()->GetCamDir();
	camDir.y = 0.f;	// Y��񂪂���Ɣ�юn�߂�̂�Y�̗v�f�𔲂�.
	D3DXVec3Normalize(&camDir, &camDir); // ���K��.

	float moveSpeed = 0.1f;	// �v���C���[�̈ړ����x.
	
	// �ړ���������x�N�g��.
	D3DXVECTOR3 forward(ZEROVEC3);
	D3DXVECTOR3 left(ZEROVEC3);
	D3DXVECTOR3 upvec(0, 1, 0);

	// ���x�N�g�������߂�.
	D3DXVec3Cross(&left, &camDir, &upvec);
	D3DXVec3Normalize(&left, &left);

	// WASD�ňړ�.
	if (Key->IsKeyDown( DIK_W )) { forward += camDir; }
	if (Key->IsKeyDown( DIK_S )) { forward -= camDir; }
	if (Key->IsKeyDown( DIK_A )) { forward += left; }
	if (Key->IsKeyDown( DIK_D )) { forward -= left; }

	// �ŏI�I�Ȉړ������𑬓x�x�N�g���ɕϊ������v�̈ړ��ʂɓn��.
	m_SumVec += forward * moveSpeed;


	//----------------------------------------------------------------------
	//		���N���b�N�Ŏˌ�.
	//----------------------------------------------------------------------
	if (Mouse->IsLAction()) {
		// �J�����̌����x�N�g�����擾.
		D3DXVECTOR3 direction = CCamera::GetInstance()->GetCamDir();
		m_pBullets.push_back(new CBullet());	// m_pBullets �ɒǉ�.

		m_pBullets.back()->AttachMesh(*m_pMeshBullet);	// ���b�V����ݒ�.
		m_pBullets.back()->SetPos(0.f, -1000.f, 0.f);	//�����ʒu�����ݒ�.

		// �x�N�g���̃m�[�}���C�Y�i�����݂̂𒊏o�j.
		D3DXVec3Normalize(&direction, &direction);

		// �����ʒu,�ړ������̒P�ʃx�N�g��,�e�̌���,���x�����邽�ߕۗ�.
		m_pBullets.back()->Init(
			m_pGun->GetShootPos(),
			CCamera::GetInstance()->GetCamDir(),
			direction,
			0.01f );
	}

	// ���v�̃x�N�g���ʕ��ʒu���X�V.
	m_vPosition += m_SumVec;
}