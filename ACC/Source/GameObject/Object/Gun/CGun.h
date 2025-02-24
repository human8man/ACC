#pragma once
#include "Mesh/Static/CStaticMeshObject.h"


//============================================================================
//		�e�N���X.
//============================================================================
class CGun
	: public CStaticMeshObject
{
public:
	CGun();
	virtual ~CGun() override;

	// �X�V����.
	virtual void Update() override;
	// �`�揈��.
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	// ���ˈʒu�̎擾.
	D3DXVECTOR3 GetShootPos() const { return m_ShootPoint; }

	/****************************************************************
	* @brief �e�̈ʒu���v���C���[�ɍ��킹�ĕύX����.
	* @param center		�v���C���[���W.
	* @param radius		�v���C���[����̋��� (���a).
	* @param playerYaw	�v���C���[�� Yaw ��]�p�x (rot.y, ���W�A���P��).
	****************************************************************/
	void UpdateGunPos( const D3DXVECTOR3& center, float radius, float playerYaw );
private:
	D3DXVECTOR3 m_ShootPoint;	// �e�̔��ˌ����W.
};