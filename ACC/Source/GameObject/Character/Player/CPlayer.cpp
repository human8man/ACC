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
	, m_MoveState	( Stop )
{
}

CPlayer::~CPlayer()
{
}


//============================================================================
//		�X�V����.
//============================================================================
void CPlayer::Update()
{

	KeyInput();

	// �O��̃t���[���Œe���΂��Ă��邩���m��Ȃ��̂�false�ɂ���.
	m_Shot = false;
	
	// ���C�̈ʒu���v���C���[�̍��W�ɂ��낦��.
	m_pRayY->Position = m_vPosition;
	// �n�ʂɂ߂荞�݉���̂��߁A�v���C���[�̈ʒu����������ɂ��Ă���.
	m_pRayY->Position.y += 0.2f;
	m_pRayY->RotationY = m_vRotation.y;

	// �\���i�O�㍶�E�ɐL�΂����j���C�̐ݒ�.
	for (int dir = 0; dir < CROSSRAY::max; dir++)
	{
		m_pCrossRay->Ray[dir].Position = m_vPosition;
		m_pCrossRay->Ray[dir].Position.y += 0.2f;
		m_pCrossRay->Ray[dir].RotationY = m_vRotation.y;
	}

	CCharacter::Update();
}


//============================================================================
//		�`�揈��.
//============================================================================
void CPlayer::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	CCharacter::Draw( View, Proj, Light );
}


//-----------------------------------------------------------------------------
//		�L�[���͏���.
//-----------------------------------------------------------------------------
void CPlayer::KeyInput()
{
	CKey* Key = CDInput::GetInstance()->CDKeyboard();
	
	// �e���΂�.
	if( Key->IsKeyAction( DIK_Z )){ m_Shot = true; }


	//----------------------------------------
	//		�ړ�����.
	//----------------------------------------
	
	// �J�����̌����x�N�g�����擾.
	D3DXVECTOR3 camDir = CCamera::GetInstance()->GetCamDir();
	camDir.y = 0.f;	//Y��񂪂���Ɣ�юn�߂�̂�Y�̗v�f�𔲂�.
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

	// �ŏI�I�Ȉړ������𑬓x�x�N�g���ɕϊ�.
	D3DXVECTOR3 velocity = forward * moveSpeed;

	// �ʒu���X�V.
	m_vPosition += velocity;
}