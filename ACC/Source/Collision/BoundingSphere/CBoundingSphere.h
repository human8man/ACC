#pragma once

#include "GameObject/Mesh/Static/CStaticMesh.h"

/**************************************************
*	バウンディングスフィアクラス.
**/
class CBoundingSphere
{
public:
	CBoundingSphere();
	~CBoundingSphere();

	//モデルに合わせたバウンディングスフィアを作成.
	HRESULT CreateSphereForMesh( const CStaticMesh& pMesh );

	//他の球体との当たり判定.
	bool IsHit( const CBoundingSphere& pBSphere );

	//中心座標を取得する.
	const D3DXVECTOR3& GetPosition() const { return m_Position;	}
	//半径(長さ)を取得する.
	float GetRadius() const			{ return m_Radius;		}

	//中心座標を設定する.
	void SetPosition( const D3DXVECTOR3& Pos )	{ m_Position = Pos;		}
	//半径(長さ)を設定する.
	void SetRadius( float Radius )				{ m_Radius = Radius;	}

private:
	D3DXVECTOR3		m_Position;	//中心座標.
	float			m_Radius;	//半径(長さ).
};