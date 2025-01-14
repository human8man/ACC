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

	// 発射位置の取得.
	D3DXVECTOR3 GetShootPos() const { return m_ShootPoint; }

private:
	D3DXVECTOR3 m_ShootPoint;

};