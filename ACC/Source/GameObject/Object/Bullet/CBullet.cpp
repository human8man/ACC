#include "CBullet.h"
#include "Time/CTime.h"


//============================================================================
//		�e�N���X.
//============================================================================
CBullet::CBullet()
	: m_MoveDirection	()
	, m_MoveSpeed		()
	, m_DeleteTime		( CTime::GetDeltaTime() * 300.f )
{
}
CBullet::~CBullet()
{
}


//============================================================================
//		�X�V����.
//============================================================================
void CBullet::Update()
{
	m_DeleteTime -= CTime::GetDeltaTime();
	m_vPosition += m_MoveDirection * m_MoveSpeed * CTime::GetDeltaTime();
}


//============================================================================
//		�`�揈��.
//============================================================================
void CBullet::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	CStaticMeshObject::Draw( View, Proj, Light );
}