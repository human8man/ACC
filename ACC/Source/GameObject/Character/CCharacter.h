#pragma once

#include "Mesh/Static/CStaticMeshObject.h"
#include "Collision/Ray/CRay.h"
#include "Object/Bullet/CBullet.h"
#include "Object/Gun/CGun.h"

//============================================================================
//		キャラクタークラス.
//============================================================================
class CCharacter
	: public CStaticMeshObject
{
public:
	CCharacter();
	virtual ~CCharacter();

	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj,LIGHT& Light) override;

	// Y軸方向へ伸ばしたレイを取得.
	RAY GetRayY() const { return *m_pRayY;  }
	// 十字レイを取得.
	CROSSRAY GetCrossRay() const { return *m_pCrossRay;  }
protected:
	RAY*		m_pRayY;		// Y方向へ伸ばしたレイ.
	CROSSRAY*	m_pCrossRay;	// 十字（前後左右に伸ばした）レイ
	
	CStaticMesh*			m_pMeshBullet;	// 弾メッシュ.
	CStaticMesh*			m_pMeshGun;		// 銃メッシュ.
	std::vector<CBullet*>	m_pBullets;		// 弾配列.
	CGun*					m_pGun;			// 銃.

	float m_GunRadius;		// 銃の半径.
	float m_GunRotRevision;	// 銃の向き補正値.
};