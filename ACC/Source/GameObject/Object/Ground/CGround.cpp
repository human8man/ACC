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
//	//�n�ʂ��X�N���[��������.
//	m_vPosition.z -= 0.2f;
//	if( m_vPosition.z < -100.0f ){
//		m_vPosition.z = 0.0f;
//	}
	
	//-------------------------
	//	Y���̃��C����.
	//-------------------------
	RAY ray = m_pPlayer->GetRayY();	//�v���C���[�������Ă��郌�C�̏��.
	float Distance = 0.0f;
	D3DXVECTOR3 Intersect( 0.0f, 0.0f, 0.0f );

	//�v���C���[�ƃ��C�Ɠ����蔻��.
	if (IsHitForRay(ray, &Distance, &Intersect) == true)
	{
		D3DXVECTOR3 Pos = m_pPlayer->GetPosition();
		Pos.y = Intersect.y + 1.0f;	//1.0f�ŏ�����֕␳.
		m_pPlayer->SetPosition( Pos );
	}

	//-------------------------
	//	�\�����C�̏���.
	//-------------------------
	CROSSRAY CrossRay = m_pPlayer->GetCrossRay();
	//�ǂƂ̓����蔻��.
	CalculatePositionFromWall( &CrossRay );
	D3DXVECTOR3 Pos = m_pPlayer->GetPosition();
	Pos.x = CrossRay.Ray[CROSSRAY::XL].Position.x;
	Pos.z = CrossRay.Ray[CROSSRAY::XL].Position.z;
	m_pPlayer->SetPosition( Pos );
}
