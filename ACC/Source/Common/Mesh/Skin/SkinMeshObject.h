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

	// 更新処理
	virtual void Update() override;
	// 描画処理
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light) override;

	// メッシュを接続する
	void AttachMesh(SkinMesh& pMesh);
	// メッシュを切り離す
	void DetachMesh();

protected:
	SkinMesh*					m_pMesh;
	LPD3DXANIMATIONCONTROLLER	m_pAnimCtrl;	// アニメーションコントローラ
};
