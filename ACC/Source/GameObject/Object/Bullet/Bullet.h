#pragma once

#include "Mesh/Static/StaticMeshObject.h"


class Bullet
	: public StaticMeshObject
{
public:
	Bullet();
	virtual ~Bullet() override;

	/****************************************************************
	// @brief 初期化処理
	// @param pos 初期位置
	// @param vector 移動方向の単位ベクトル
	// @param speed 移動速度
	****************************************************************/
	void Init(
		const D3DXVECTOR3& pos,
		const D3DXVECTOR3& vector,
		const float& speed )
	{
		m_vPosition = pos;
		m_MoveDirection = vector;
		m_MoveSpeed = speed;
	};

	// 更新処理
	virtual void Update() override;

	// 描画処理
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	// 弾が削除可能か
	bool DeleteBullet() const { return (m_DeleteTime < 0); }
	D3DXVECTOR3 GetMoveVec() const { return m_MoveDirection; }

private:
	D3DXVECTOR3	m_MoveDirection;	// 移動方向
	float		m_MoveSpeed;		// 移動速度
	float		m_DeleteTime;		// 解放までの時間
};
