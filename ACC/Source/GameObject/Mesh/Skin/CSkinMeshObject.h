#pragma once

#include "CSkinMesh.h"
#include "GameObject/CGameObject.h"
#include "Collision/BoundingSphere/CBoundingSphere.h"
#include "../Static/CStaticMesh.h"

/************************************************************
*	スキンメッシュオブジェクトクラス.
**/
class CSkinMeshObject
	: public CGameObject
{
public:
	CSkinMeshObject();
	virtual ~CSkinMeshObject() override;

	//CGameObjectで純粋仮想関数の宣言がされてるのでこちらで定義を書く.
	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light) override;

	//メッシュを接続する.
	void AttachMesh(CSkinMesh& pMesh);
	//メッシュを切り離す.
	void DetachMesh();

	//バウンディングスフィア取得.
	CBoundingSphere* GetBSphere() const {
		return m_pBSphere;
	}
	//モデルに合わせたバウンディングスフィア作成のラッパー関数.
	HRESULT CreateBSphereForMesh(const CStaticMesh& pMesh) {
		return m_pBSphere->CreateSphereForMesh(pMesh);
	}
	//バウンディングスフィアをオブジェクト位置に合わせる.
	//※モデルの原点が中心の場合を想定.
	void UpdateBSpherePos() {
		m_pBSphere->SetPosition(m_vPosition);
	}

protected:
	CSkinMesh*					m_pMesh;
	CBoundingSphere*			m_pBSphere;
	LPD3DXANIMATIONCONTROLLER	m_pAnimCtrl;	//アニメーションコントローラ.
};
