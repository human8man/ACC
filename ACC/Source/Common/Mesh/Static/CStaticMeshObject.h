#pragma once

#include "CStaticMesh.h"
#include "GameObject.h"
#include "Collision/Ray/CRay.h"
#include <tuple>


//============================================================================
//		スタティックメッシュオブジェクトクラス
//============================================================================
class CStaticMeshObject
	: public GameObject
{
public:
	CStaticMeshObject();
	virtual ~CStaticMeshObject() override;

	// 更新処理
	virtual void Update() override;
	// 描画処理
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	// メッシュを接続
	void AttachMesh( CStaticMesh& pMesh ){ m_pMesh = &pMesh; }
	// メッシュを解放
	void DetachMesh(){ m_pMesh = nullptr; }
	
	// メッシュの取得
	CStaticMesh* GetMesh() const { return m_pMesh; }

	// 壁からの位置を計算する
	void CalculatePositionFromWall( CROSSRAY* pCrossRay );

	// レイとメッシュの当たり判定(Hit判定、交差点、長さを返す)
	RayInfo IsHitForRay( const RAY& pRay  );
	
private:
	// 交差位置のポリゴンの頂点を見つける
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMesh,
		DWORD dwPolyIndex,
		D3DXVECTOR3* pVertices);

	// 回転値調整（１周以上している時の調整）
	void ClampDirection(float* dir);

protected:
	CStaticMesh* m_pMesh;
};