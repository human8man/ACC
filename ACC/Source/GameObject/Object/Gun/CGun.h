#pragma once
#include "GameObject/Mesh/Static/CStaticMeshObject.h"


//============================================================================
//		銃クラス.
//============================================================================
class CGun
	: public CStaticMeshObject
{
public:
	CGun();
	virtual ~CGun() override;

	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

	// 発射位置の設定.
	void SetShootPos(D3DXVECTOR3& pos) { m_ShootPoint = pos; }
	void SetShootPos(float x, float y, float z) {
		m_ShootPoint.x = x;
		m_ShootPoint.y = y;
		m_ShootPoint.z = z;
	};

	// 発射位置の取得.
	D3DXVECTOR3 GetShootPos() const { return m_ShootPoint; }
	
	// 銃の位置をプレイヤーに合わせて変更する.
	void UpdateGunPos(
		const D3DXVECTOR3& center,	// 基準点（プレイヤー）.
		float radius,				// 半径.
		float playerYaw);			// プレイヤーのYaw軸（rot.y）.
private:
	D3DXVECTOR3 m_ShootPoint;
};