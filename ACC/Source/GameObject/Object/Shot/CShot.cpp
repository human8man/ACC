#include "CShot.h"

CShot::CShot()
	: m_Display			( false )
	, m_MoveDirection	()
	, m_MoveSpeed		()
	, m_DisplayTime		()
{
}

CShot::~CShot()
{
}

void CShot::Update()
{
	if( m_Display == true ){
		//�ړ������Ɉړ����x���|�����킹�����̂����W�ɔ��f.
		m_vPosition += m_MoveDirection * m_MoveSpeed;

		m_DisplayTime--;
		if( m_DisplayTime < 0 ){
			//�����Ȃ����ɒu���Ă���.
			m_vPosition = D3DXVECTOR3( 0.0f, -10.0f, 0.0f );
			m_Display = false;
		}
	}
}

void CShot::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	if( m_Display == true ){
		CStaticMeshObject::Draw( View, Proj, Light );
	}
}

void CShot::Reload( const D3DXVECTOR3& Pos, float RotY )
{
	//�e�����ł�����I��.
	if( m_Display == true )	return;

	m_vPosition		= Pos;
	m_Display		= true;
	m_vRotation.y	= RotY;		//�e�̌����i�����ځj���ς���.
	m_MoveSpeed		= 0.2f;		//�ړ����x�Ƃ肠����0.2f��ݒ�.
	m_DisplayTime	= FPS * 3;	//��3�b���炢�\������.

	//Z���x�N�g��.
	D3DXVECTOR3 vecAxisZ( 0.0f, 0.0f, 1.0f );

	//Y�����̉�]�s��.
	D3DXMATRIX mRotationY;
	//Y����]�s����쐬.
	D3DXMatrixRotationY(
		&mRotationY,		//(out)�s��.
		m_vRotation.y );	//�v���C���[��Y�����̉�]�l.

	//Y����]�s����g����Z���x�N�g�������W�ϊ�����.
	D3DXVec3TransformCoord(
		&m_MoveDirection,	//(out)�ړ�����.
		&vecAxisZ,			//(in)Z���x�N�g��.
		&mRotationY );		//Y����]�s��.
}
