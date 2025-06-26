#pragma once

#include "CStaticMesh.h"
#include "GameObject.h"
#include "Collision/Ray/CRay.h"
#include <tuple>


//============================================================================
//		�X�^�e�B�b�N���b�V���I�u�W�F�N�g�N���X
//============================================================================
class CStaticMeshObject
	: public GameObject
{
public:
	CStaticMeshObject();
	virtual ~CStaticMeshObject() override;

	// �X�V����
	virtual void Update() override;
	// �`�揈��
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	// ���b�V����ڑ�
	void AttachMesh( CStaticMesh& pMesh ){ m_pMesh = &pMesh; }
	// ���b�V�������
	void DetachMesh(){ m_pMesh = nullptr; }
	
	// ���b�V���̎擾
	CStaticMesh* GetMesh() const { return m_pMesh; }

	// �ǂ���̈ʒu���v�Z����
	void CalculatePositionFromWall( CROSSRAY* pCrossRay );

	// ���C�ƃ��b�V���̓����蔻��(Hit����A�����_�A������Ԃ�)
	RayInfo IsHitForRay( const RAY& pRay  );
	
private:
	// �����ʒu�̃|���S���̒��_��������
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMesh,
		DWORD dwPolyIndex,
		D3DXVECTOR3* pVertices);

	// ��]�l�����i�P���ȏサ�Ă��鎞�̒����j
	void ClampDirection(float* dir);

protected:
	CStaticMesh* m_pMesh;
};