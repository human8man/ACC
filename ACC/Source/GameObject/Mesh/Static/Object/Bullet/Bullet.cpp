#include "Bullet.h"
#include "Time/Time.h"


Bullet::Bullet()
	: m_MoveDirection	()
	, m_MoveSpeed		()
	, m_DeleteTime		( Time::GetDeltaTime() * 70.f )
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
	m_DeleteTime -= Time::GetDeltaTime() * Time::GetTimeScale();
	m_vPosition += m_MoveDirection * m_MoveSpeed;
}


//============================================================================
//		�`�揈��
//============================================================================
void Bullet::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	StaticMeshObject::Draw( View, Proj, Light );
}