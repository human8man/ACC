#pragma once
#include "GameObject/Mesh/Static/CStaticMeshObject.h"


//============================================================================
//		�e�N���X.
//============================================================================
class CBullet
	: public CStaticMeshObject
{
public:
	CBullet();
	virtual ~CBullet() override;

	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	// ������(�����ʒu,�ړ������̒P�ʃx�N�g��,���x).
	void Init(
		const D3DXVECTOR3& pos,
		const D3DXVECTOR3& vector,
		const float& speed );

	// �e���폜�\��.
	bool DeleteBullet() const { return (m_DeleteTime < 0); }
private:
	D3DXVECTOR3	m_MoveDirection;	// �ړ�����.
	float		m_MoveSpeed;		// �ړ����x.
	int			m_DeleteTime;		// ����܂ł̎���.
};
