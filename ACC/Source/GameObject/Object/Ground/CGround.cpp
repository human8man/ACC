#include "CGround.h"

CGround::CGround()
	: m_pPlayer		( nullptr )
{
}

CGround::~CGround()
{
	m_pPlayer = nullptr;
}

void CGround::Update()
{
//	//地面をスクロールさせる.
//	m_vPosition.z -= 0.2f;
//	if( m_vPosition.z < -100.0f ){
//		m_vPosition.z = 0.0f;
//	}
	
	//-------------------------
	//	Y軸のレイ処理.
	//-------------------------
	RAY ray = m_pPlayer->GetRayY();	//プレイヤーが持っているレイの情報.
	float Distance = 0.0f;
	D3DXVECTOR3 Intersect( 0.0f, 0.0f, 0.0f );

	//プレイヤーとレイと当たり判定.
	if (IsHitForRay(ray, &Distance, &Intersect) == true)
	{
		D3DXVECTOR3 Pos = m_pPlayer->GetPosition();
		Pos.y = Intersect.y + 1.0f;	//1.0fで少し上へ補正.
		m_pPlayer->SetPosition( Pos );
	}

	//-------------------------
	//	十字レイの処理.
	//-------------------------
	CROSSRAY CrossRay = m_pPlayer->GetCrossRay();
	//壁との当たり判定.
	CalculatePositionFromWall( &CrossRay );
	D3DXVECTOR3 Pos = m_pPlayer->GetPosition();
	Pos.x = CrossRay.Ray[CROSSRAY::XL].Position.x;
	Pos.z = CrossRay.Ray[CROSSRAY::XL].Position.z;
	m_pPlayer->SetPosition( Pos );
}
