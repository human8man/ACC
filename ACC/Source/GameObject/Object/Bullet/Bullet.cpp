#include "Bullet.h"
#include "Time/CTime.h"


Bullet::Bullet()
	: m_MoveDirection	()
	, m_MoveSpeed		()
	, m_DeleteTime		( CTime::GetDeltaTime() * 70.f )
{
}
Bullet::~Bullet()
{
}


//============================================================================
//		�X�V����
//============================================================================
void Bullet::Update()
{
	m_DeleteTime -= CTime::GetDeltaTime() * CTime::GetTimeScale();
	m_vPosition += m_MoveDirection * m_MoveSpeed * CTime::GetDeltaTime();
}


//============================================================================
//		�`�揈��
//============================================================================
void Bullet::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	CStaticMeshObject::Draw( View, Proj, Light );
}