#pragma once

#include "Mesh/Static/StaticMeshObject.h"


class Bullet
	: public StaticMeshObject
{
public:
	Bullet();
	virtual ~Bullet() override;

	/****************************************************************
	// @brief ����������
	// @param pos �����ʒu
	// @param vector �ړ������̒P�ʃx�N�g��
	// @param speed �ړ����x
	****************************************************************/
	void Init(
		const D3DXVECTOR3& pos,
		const D3DXVECTOR3& vector,
		const float& speed )
	{
		m_vPosition = pos;
		m_MoveDirection = vector;
		m_MoveSpeed = speed;
	};

	// �X�V����
	virtual void Update() override;

	// �`�揈��
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	// �e���폜�\��
	bool DeleteBullet() const { return (m_DeleteTime < 0); }
	D3DXVECTOR3 GetMoveVec() const { return m_MoveDirection; }

private:
	D3DXVECTOR3	m_MoveDirection;	// �ړ�����
	float		m_MoveSpeed;		// �ړ����x
	float		m_DeleteTime;		// ����܂ł̎���
};
