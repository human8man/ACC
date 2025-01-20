#pragma once

#include "CStaticMesh.h"
#include "CGameObject.h"
#include "Collision/BoundingSphere/CBoundingSphere.h"
#include "Collision/Ray/CRay.h"
#include <tuple>

//============================================================================
//		�X�^�e�B�b�N���b�V���I�u�W�F�N�g�N���X.
//============================================================================
class CStaticMeshObject
	: public CGameObject
{
public:
	CStaticMeshObject();
	virtual ~CStaticMeshObject() override;

	// CGameObject�ŏ������z�֐��̐錾������Ă�̂ł�����Œ�`������.
	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	//���b�V����ڑ�.
	void AttachMesh( CStaticMesh& pMesh ){ m_pMesh = &pMesh; }
	//���b�V�������.
	void DetachMesh(){ m_pMesh = nullptr; }
	
	// �o�E���f�B���O�X�t�B�A���I�u�W�F�N�g�ʒu�ɍ��킹��.
	void UpdateBSpherePos() { m_pBSphere->SetPosition( m_vPosition ); }

	// �o�E���f�B���O�X�t�B�A�擾.
	CBoundingSphere* GetBSphere() const { return m_pBSphere; }
	// ���b�V���̎擾.
	CStaticMesh*	 GetMesh()	  const { return m_pMesh;	 }

	// ���f���ɍ��킹���o�E���f�B���O�X�t�B�A�쐬�̃��b�p�[�֐�.
	HRESULT CreateBSphereForMesh( const CStaticMesh& pMesh ){
		return m_pBSphere->CreateSphereForMesh( pMesh );
	}
	
	// �ǂ���̈ʒu���v�Z����.
	void CalculatePositionFromWall( CROSSRAY* pCrossRay );

	// ���C�ƃ��b�V���̓����蔻��(Hit����A�����_�A������Ԃ�).
	RayInfo IsHitForRay( const RAY& pRay  );
	
private:
	// �����ʒu�̃|���S���̒��_��������.
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMesh,
		DWORD dwPolyIndex,
		D3DXVECTOR3* pVertices);

	// ��]�l�����i�P���ȏサ�Ă��鎞�̒����j.
	void ClampDirection(float* dir);

protected:
	CStaticMesh*		m_pMesh;
	CBoundingSphere*	m_pBSphere;
};
