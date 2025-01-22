#include "CBullet.h"

//============================================================================
//		�e�N���X.
//============================================================================
CBullet::CBullet()
	: m_MoveDirection	()
	, m_MoveSpeed		()
	, m_DeleteTime		(FPS * 10)
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
	m_DeleteTime--;
	m_vPosition += m_MoveDirection * m_MoveSpeed;
}


//============================================================================
//		�`�揈��.
//============================================================================
void CBullet::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	CStaticMeshObject::Draw( View, Proj, Light );
}


//============================================================================
//		������.
//============================================================================
void CBullet::Init(
	const D3DXVECTOR3& pos,
	const D3DXVECTOR3& vector,
	const float& speed)
{
	m_vPosition		= pos;
	m_MoveDirection = vector;
	m_MoveSpeed		= speed;
}