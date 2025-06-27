#pragma once

#include "Mesh/Static/StaticMeshObject.h"


class Gun
	: public StaticMeshObject
{
public:
	Gun();
	virtual ~Gun() override;

	// 更新処理
	virtual void Update() override;
	// 描画処理
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	// 発射位置の取得
	D3DXVECTOR3 GetShootPos() const { return m_ShootPoint; }

	/****************************************************************
	* @brief 銃の位置をプレイヤーに合わせて変更する
	* @param center		: オブジェクト座標
	* @param radius		: オブジェクトからの距離 (半径)
	* @param playerYaw	: オブジェクトの Yaw 回転角度 (rot.y, ラジアン単位)
	****************************************************************/
	void UpdateGunPos( const D3DXVECTOR3& center, float radius, float playerYaw );
private:
	D3DXVECTOR3 m_ShootPoint;	// 銃の発射口座標
};