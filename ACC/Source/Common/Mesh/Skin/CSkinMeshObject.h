#pragma once

#include "CSkinMesh.h"
#include "CGameObject.h"
#include "Mesh/Static/CStaticMesh.h"


//=============================================================================
//		スキンメッシュオブジェクトクラス.
//=============================================================================
class CSkinMeshObject
	: public CGameObject
{
public:
	CSkinMeshObject();
	virtual ~CSkinMeshObject() override;

	// 更新処理.
	virtual void Update() override;
	// 描画処理.
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light) override;

	// メッシュを接続する.
	void AttachMesh(CSkinMesh& pMesh);
	// メッシュを切り離す.
	void DetachMesh();

protected:
	CSkinMesh*					m_pMesh;
	LPD3DXANIMATIONCONTROLLER	m_pAnimCtrl;	// アニメーションコントローラ.
};
