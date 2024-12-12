#include "CPlayer.h"
#include "DirectSound/CSoundManager.h"

CPlayer::CPlayer()
	: m_TurnSpeed	( 0.1f )
	, m_MoveSpeed	( 0.1f )
	, m_MoveState	( Stop )
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::Update()
{
	//�O��̃t���[���Œe���΂��Ă��邩���m��Ȃ��̂�false�ɂ���.
	m_Shot = false;

	//�e���΂�����!!.
	if( GetAsyncKeyState( 'Z' ) & 0x8000 ){
		m_Shot = true;
	}

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

void CPlayer::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj,
	LIGHT& Light )
{
	CCharacter::Draw( View, Proj, Light );
}

//���W�R������.
void CPlayer::RadioControl()
{
	//Z���x�N�g���iZ+�����ւ̒P�ʃx�N�g���j.
	//�������i�傫���j���P�̃x�N�g����P�ʃx�N�g���Ƃ���.
	D3DXVECTOR3 vecAxisZ( 0.0f, 0.0f, 1.0f );

	//Y�����̉�]�s��.
	D3DXMATRIX mRotationY;
	//Y����]�s����쐬.
	D3DXMatrixRotationY(
		&mRotationY,		//(out)�s��.
		m_vRotation.y );	//�v���C���[��Y�����̉�]�l.

	//Y����]�s����g����Z���x�N�g�������W�ϊ�����.
	D3DXVec3TransformCoord(
		&vecAxisZ,		//(out)Z���x�N�g��.
		&vecAxisZ,		//(in)Z���x�N�g��.
		&mRotationY );	//Y����]�s��.

	switch( m_MoveState ){
	case enMoveState::Forward:	//�O�i.
		m_vPosition += vecAxisZ * m_MoveSpeed;
		break;
	case enMoveState::Backward:	//���.
		m_vPosition -= vecAxisZ * m_MoveSpeed;
		break;
	default:
		break;
	}
	//��L�̈ړ��������I���Β�~��Ԃɂ��Ă���.
	m_MoveState = enMoveState::Stop;
}
