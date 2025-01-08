#include "CBullet.h"

//============================================================================
//		弾クラス.
//============================================================================
CBullet::CBullet()
	: m_MoveDirection	()
	, m_MoveSpeed		()
	, m_DeleteTime		()
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
	m_DeleteTime--;
	m_vPosition += m_MoveDirection * m_MoveSpeed;
}


//============================================================================
//		描画処理.
//============================================================================
void CBullet::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	CStaticMeshObject::Draw( View, Proj, Light );
}


//============================================================================
//		初期化.
//============================================================================
void CBullet::Init(
	const D3DXVECTOR3& pos,
	const D3DXVECTOR3& vector,
	const D3DXVECTOR3& bulletvector,
	const float& speed)
{
	m_vPosition		= pos;
	m_MoveDirection = vector;
	m_vRotation		= bulletvector;
	m_MoveSpeed		= speed;
	m_DeleteTime	= FPS * 10;
}