#include "CGround.h"

//============================================================================
//		�n�ʃN���X.
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
//		�X�V����.
//============================================================================
void CGround::Update()
{
	//-------------------------
	//	Y���̃��C����.
	//-------------------------
	RAY ray = m_pPlayer->GetRayY();	// �v���C���[�������Ă��郌�C�̏��.
	float Distance = 0.0f;
	D3DXVECTOR3 Intersect( 0.0f, 0.0f, 0.0f );

	// �v���C���[�ƃ��C�Ɠ����蔻��.
	auto [hit, hitpos, length] = IsHitForRay(ray);
	if ( hit )
	{
		D3DXVECTOR3 Pos = m_pPlayer->GetPos();
		Pos.y = hitpos.y + 1.0f;	// 1.0f�ŏ�����֕␳.
		m_pPlayer->SetPos( Pos );
	}


	//-------------------------
	//	�\�����C�̏���.
	//-------------------------
	CROSSRAY CrossRay = m_pPlayer->GetCrossRay();
	// �ǂƂ̓����蔻��.
	CalculatePositionFromWall( &CrossRay );
	D3DXVECTOR3 Pos = m_pPlayer->GetPos();
	Pos.x = CrossRay.Ray[CROSSRAY::XL].Position.x;
	Pos.z = CrossRay.Ray[CROSSRAY::XL].Position.z;
	m_pPlayer->SetPos( Pos );
}
