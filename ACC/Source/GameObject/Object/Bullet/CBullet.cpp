#include "CBullet.h"
#include "Time/CTime.h"


//============================================================================
//		弾クラス.
//============================================================================
CBullet::CBullet()
	: m_MoveDirection	()
	, m_MoveSpeed		()
	, m_DeleteTime		( CTime::GetInstance()->GetDeltaTime() * 600.f )
{
}
CBullet::~CBullet()
{
}


//============================================================================
//		更新処理.
//============================================================================
void CBullet::Update()
{
	m_DeleteTime -= CTime::GetInstance()->GetDeltaTime();
	m_vPosition += m_MoveDirection * m_MoveSpeed;
}


//============================================================================
//		描画処理.
//============================================================================
void CBullet::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	CStaticMeshObject::Draw( View, Proj, Light );
}