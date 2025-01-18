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
	void SetShootPos(float x, float y, float z) {
		m_ShootPoint.x = x;
		m_ShootPoint.y = y;
		m_ShootPoint.z = z;
	};

	// ���ˈʒu�̎擾.
	D3DXVECTOR3 GetShootPos() const { return m_ShootPoint; }
	
	// �e�̈ʒu���v���C���[�ɍ��킹�ĕύX����.
	void UpdateGunPos(
		const D3DXVECTOR3& center,	// ��_�i�v���C���[�j.
		float radius,				// ���a.
		float playerYaw);			// �v���C���[��Yaw���irot.y�j.
private:
	D3DXVECTOR3 m_ShootPoint;
};