#pragma once

#include "CStaticMesh.h"
#include "../GameObject/CGameObject.h"
#include "../Collision/BoundingSphere/CBoundingSphere.h"
#include "../Collision/Ray/CRay.h"

/************************************************************
*	スタティックメッシュオブジェクトクラス.
**/
class CStaticMeshObject
	: public CGameObject
{
public:
	CStaticMeshObject();
	virtual ~CStaticMeshObject() override;

	//CGameObjectで純粋仮想関数の宣言がされてるのでこちらで定義を書く.
	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj,
		LIGHT& Light, CAMERA& Camera ) override;

	//メッシュを接続する.
	void AttachMesh( CStaticMesh& pMesh ){
		m_pMesh = &pMesh;
	}
	//メッシュを切り離す.
	void DetachMesh(){
		m_pMesh = nullptr;
	}
	//バウンディングスフィア取得.
	CBoundingSphere* GetBSphere() const {
		return m_pBSphere;
	}
	//モデルに合わせたバウンディングスフィア作成のラッパー関数.
	HRESULT CreateBSphereForMesh( const CStaticMesh& pMesh ){
		return m_pBSphere->CreateSphereForMesh( pMesh );
	}
	//バウンディングスフィアをオブジェクト位置に合わせる.
	//※モデルの原点が中心の場合を想定.
	void UpdateBSpherePos() {
		m_pBSphere->SetPosition( m_vPosition );
	}

	//レイとメッシュの当たり判定.
	bool IsHitForRay(
		const RAY& pRay,			//レイ構造体.
		float* pfDistance,			//(out)距離.
		D3DXVECTOR3* pvIntersect );	//(out)交差点.

	//壁からの位置を計算する.
	void CalculatePositionFromWall( CROSSRAY* pCrossRay );

private:
	//交差位置のポリゴンの頂点を見つける.
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMesh,
		DWORD dwPolyIndex,
		D3DXVECTOR3* pVertices);	//(out)頂点情報.

	//回転値調整（１周以上している時の調整）.
	void ClampDirection(float* dir);	//再帰関数.

protected:
	CStaticMesh*		m_pMesh;
	CBoundingSphere*	m_pBSphere;
};
