#include "CPlayer.h"
#include "DirectSound/CSoundManager.h"

CPlayer::CPlayer()
	: m_TurnSpeed		( 0.1f )	//���������肽���ꍇ�̓��W�A���l��ݒ肷�邱��.
	, m_MoveSpeed		( 0.1f )
	, m_MoveState		( Stop )
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::Update()
{
#if 1
	//�O�i.
	if( GetAsyncKeyState( VK_UP ) & 0x8000 ){
		m_MoveState = enMoveState::Forward;
	}
	//���.
	if( GetAsyncKeyState( VK_DOWN ) & 0x8000 ){
		m_MoveState = enMoveState::Backward;
	}
	if( GetAsyncKeyState( VK_RIGHT ) & 0x8000 ){
		m_vRotation.y += m_TurnSpeed;
	}
	if( GetAsyncKeyState( VK_LEFT ) & 0x8000 ){
		m_vRotation.y -= m_TurnSpeed;
	}

	RadioControl();

#else
	float add_value = 0.1f;
	if( GetAsyncKeyState( VK_UP ) & 0x8000 ){
		m_vPosition.y += add_value;
	}
	if( GetAsyncKeyState( VK_DOWN ) & 0x8000 ){
		m_vPosition.y -= add_value;
	}
	if( GetAsyncKeyState( VK_RIGHT ) & 0x8000 ){
		m_vPosition.x += add_value;
	}
	if( GetAsyncKeyState( VK_LEFT ) & 0x8000 ){
		m_vPosition.x -= add_value;
	}
#endif
	//�O��̃t���[���Œe���΂��Ă��邩���m��Ȃ��̂�false�ɂ���.
	m_Shot = false;

	//�e���΂�����!!.
	if( GetAsyncKeyState( 'Z' ) & 0x8000 ){
		m_Shot = true;

		//SE�̍Đ�.
		CSoundManager::PlaySE( CSoundManager::SE_Jump );
	}

	//���C�̈ʒu���v���C���[�̍��W�ɂ��낦��.
	m_pRayY->Position = m_vPosition;
	//�n�ʂɂ߂荞�݉���̂��߁A�v���C���[�̈ʒu����������ɂ��Ă���.
	m_pRayY->Position.y += 0.2f;
	m_pRayY->RotationY = m_vRotation.y;

	//�\���i�O�㍶�E�ɐL�΂����j���C�̐ݒ�.
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
