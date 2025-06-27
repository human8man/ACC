#pragma once

#include "SkinMesh.h"
#include "GameObject.h"
#include "Mesh/Static/StaticMesh.h"


class SkinMeshObject
	: public GameObject
{
public:
	SkinMeshObject();
	virtual ~SkinMeshObject() override;

	// �X�V����
	virtual void Update() override;
	// �`�揈��
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light) override;

	// ���b�V����ڑ�����
	void AttachMesh(SkinMesh& pMesh);
	// ���b�V����؂藣��
	void DetachMesh();

protected:
	SkinMesh*					m_pMesh;
	LPD3DXANIMATIONCONTROLLER	m_pAnimCtrl;	// �A�j���[�V�����R���g���[��
};
