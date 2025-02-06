#pragma once

#include "GameObject/Mesh/Static/CStaticMeshObject.h"


//============================================================================
//		弾クラス.
//============================================================================
class CBullet
	: public CStaticMeshObject
{
public:
	CBullet();
	virtual ~CBullet() override;
	
	// 初期化処理(初期位置,移動方向の単位ベクトル,速度).
	void Init(
		const D3DXVECTOR3& pos,
		const D3DXVECTOR3& vector,
		const float& speed );
	// 更新処理.
	virtual void Update() override;
	// 描画処理.
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	// 弾が削除可能か.
	bool DeleteBullet() const { return (m_DeleteTime < 0); }	

private:
	D3DXVECTOR3	m_MoveDirection;	// 移動方向.
	float		m_MoveSpeed;		// 移動速度.
	int			m_DeleteTime;		// 解放までの時間.
};
