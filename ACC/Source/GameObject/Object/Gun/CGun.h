#pragma once
#include "GameObject/Mesh/Static/CStaticMeshObject.h"


//============================================================================
//		�e�N���X.
//============================================================================
class CGun
	: public CStaticMeshObject
{
public:
	CGun();
	virtual ~CGun() override;

	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	// ���ˈʒu�̐ݒ�.
	void SetShootPos(D3DXVECTOR3& pos) { m_ShootPoint = pos; }

	// ���ˈʒu�̎擾.
	D3DXVECTOR3 GetShootPos() const { return m_ShootPoint; }

private:
	D3DXVECTOR3 m_ShootPoint;

};