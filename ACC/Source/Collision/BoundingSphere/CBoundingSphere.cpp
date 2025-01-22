#include "CBoundingSphere.h"

CBoundingSphere::CBoundingSphere()
	: m_Position	()
	, m_Radius		()
{
}

CBoundingSphere::~CBoundingSphere()
{
}

//モデルに合わせたバウンディングスフィアを作成.
HRESULT CBoundingSphere::CreateSphereForMesh( const CStaticMesh& pMesh )
{
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;	//頂点バッファ.
	void* pVertices = nullptr;				//頂点.
	D3DXVECTOR3 Center( 0.0f, 0.0f, 0.0f );	//中心座標.
	float Radius = 0.0f;					//半径.

	//頂点バッファを取得.
	if (FAILED(
		pMesh.GetMesh()->GetVertexBuffer(&pVB)))
	{
		return E_FAIL;
	}

	//メッシュの頂点バッファをロックする.
	if (FAILED(
		pVB->Lock(0, 0, &pVertices, 0)))
	{
		SAFE_RELEASE( pVB );
		return E_FAIL;
	}

	//メッシュの外接円の中心と半径を計算する.
	D3DXComputeBoundingSphere(
		static_cast<D3DXVECTOR3*>( pVertices ),
		pMesh.GetMesh()->GetNumVertices(),					//頂点の数.
		D3DXGetFVFVertexSize(pMesh.GetMesh()->GetFVF() ),	//頂点の情報.
		&Center,											//(out)中心座標.
		&Radius	);											//(out)半径.

	//メッシュの頂点バッファをアンロックする.
	if (pVB != nullptr) {
		pVB->Unlock();
		SAFE_RELEASE( pVB );
	}

	//中心座標と半径を設定.
	m_Position	= Center;
	m_Radius	= Radius;

	return S_OK;
}

//他の球体との当たり判定.
bool CBoundingSphere::IsHit( const CBoundingSphere& pBSphere )
{
	//２つの球体の中心間の距離を求める.
	D3DXVECTOR3 vLength = m_Position - pBSphere.GetPosition();
	//上記のベクトルから長さに変換.
	float Length = D3DXVec3Length( &vLength );

	//「２つの球体間の距離」が「２つの球体のそれぞれの半径を足したもの」より、
	//小さいということは、球体同士が重なっている（衝突している）ということ.
	if (Length <= m_Radius + pBSphere.GetRadius())
	{
		return true;	//衝突している.
	}
	return false;		//衝突していない.
}
