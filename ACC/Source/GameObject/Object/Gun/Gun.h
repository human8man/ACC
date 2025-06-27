#pragma once

#include "Mesh/Static/StaticMeshObject.h"


class Gun
	: public StaticMeshObject
{
public:
	Gun();
	virtual ~Gun() override;

	// �X�V����
	virtual void Update() override;
	// �`�揈��
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	// ���ˈʒu�̎擾
	D3DXVECTOR3 GetShootPos() const { return m_ShootPoint; }

	/****************************************************************
	* @brief �e�̈ʒu���v���C���[�ɍ��킹�ĕύX����
	* @param center		: �I�u�W�F�N�g���W
	* @param radius		: �I�u�W�F�N�g����̋��� (���a)
	* @param playerYaw	: �I�u�W�F�N�g�� Yaw ��]�p�x (rot.y, ���W�A���P��)
	****************************************************************/
	void UpdateGunPos( const D3DXVECTOR3& center, float radius, float playerYaw );
private:
	D3DXVECTOR3 m_ShootPoint;	// �e�̔��ˌ����W
};