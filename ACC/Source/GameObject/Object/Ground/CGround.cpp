#include "CGround.h"

//============================================================================
//		地面クラス.
//============================================================================
CGround::CGround()
	: m_pPlayer		( nullptr )
{
}

CGround::~CGround()
{
	m_pPlayer = nullptr;
}


//============================================================================
//		更新処理.
//============================================================================
void CGround::Update()
{
	//-------------------------
	//	Y軸のレイ処理.
	//-------------------------
	RAY ray = m_pPlayer->GetRayY();	// プレイヤーが持っているレイの情報.
	float Distance = 0.0f;
	D3DXVECTOR3 Intersect( 0.0f, 0.0f, 0.0f );

	// プレイヤーとレイと当たり判定.
	auto [hit, hitpos, length] = IsHitForRay(ray);
	if ( hit )
	{
		D3DXVECTOR3 Pos = m_pPlayer->GetPos();
		Pos.y = hitpos.y + 1.0f;	// 1.0fで少し上へ補正.
		m_pPlayer->SetPos( Pos );
	}


	//-------------------------
	//	十字レイの処理.
	//-------------------------
	CROSSRAY CrossRay = m_pPlayer->GetCrossRay();
	// 壁との当たり判定.
	CalculatePositionFromWall( &CrossRay );
	D3DXVECTOR3 Pos = m_pPlayer->GetPos();
	Pos.x = CrossRay.Ray[CROSSRAY::XL].Position.x;
	Pos.z = CrossRay.Ray[CROSSRAY::XL].Position.z;
	m_pPlayer->SetPos( Pos );
}
