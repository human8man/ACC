#pragma once

#include "CStaticMesh.h"
#include "CGameObject.h"
#include "Collision/BoundingSphere/CBoundingSphere.h"
#include "Collision/Ray/CRay.h"
#include <tuple>

//============================================================================
//		スタティックメッシュオブジェクトクラス.
//============================================================================
class CStaticMeshObject
	: public CGameObject
{
public:
	CStaticMeshObject();
	virtual ~CStaticMeshObject() override;

	// CGameObjectで純粋仮想関数の宣言がされてるのでこちらで定義を書く.
	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	//メッシュを接続.
	void AttachMesh( CStaticMesh& pMesh ){ m_pMesh = &pMesh; }
	//メッシュを解放.
	void DetachMesh(){ m_pMesh = nullptr; }
	
	// バウンディングスフィアをオブジェクト位置に合わせる.
	void UpdateBSpherePos() { m_pBSphere->SetPosition( m_vPosition ); }

	// バウンディングスフィア取得.
	CBoundingSphere* GetBSphere() const { return m_pBSphere; }
	// メッシュの取得.
	CStaticMesh*	 GetMesh()	  const { return m_pMesh;	 }

	// モデルに合わせたバウンディングスフィア作成のラッパー関数.
	HRESULT CreateBSphereForMesh( const CStaticMesh& pMesh ){
		return m_pBSphere->CreateSphereForMesh( pMesh );
	}
	
	// 壁からの位置を計算する.
	void CalculatePositionFromWall( CROSSRAY* pCrossRay );

	// レイとメッシュの当たり判定(Hit判定、交差点、長さを返す).
	RayInfo IsHitForRay( const RAY& pRay  );
	
private:
	// 交差位置のポリゴンの頂点を見つける.
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMesh,
		DWORD dwPolyIndex,
		D3DXVECTOR3* pVertices);

	// 回転値調整（１周以上している時の調整）.
	void ClampDirection(float* dir);

protected:
	CStaticMesh*		m_pMesh;
	CBoundingSphere*	m_pBSphere;
};
