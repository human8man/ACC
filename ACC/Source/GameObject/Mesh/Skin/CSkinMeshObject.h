#pragma once

#include "CSkinMesh.h"
#include "CGameObject.h"
#include "Mesh/Static/CStaticMesh.h"


//=============================================================================
//		�X�L�����b�V���I�u�W�F�N�g�N���X.
//=============================================================================
class CSkinMeshObject
	: public CGameObject
{
public:
	CSkinMeshObject();
	virtual ~CSkinMeshObject() override;

	// �X�V����.
	virtual void Update() override;
	// �`�揈��.
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light) override;

	// ���b�V����ڑ�����.
	void AttachMesh(CSkinMesh& pMesh);
	// ���b�V����؂藣��.
	void DetachMesh();

protected:
	CSkinMesh*					m_pMesh;
	LPD3DXANIMATIONCONTROLLER	m_pAnimCtrl;	// �A�j���[�V�����R���g���[��.
};
