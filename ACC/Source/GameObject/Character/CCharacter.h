#pragma once

#include "GameObject/Mesh/Static/CStaticMeshObject.h"
#include "Collision/Ray/CRay.h"

/**************************************************
*	キャラクタークラス.
**/
class CCharacter
	: public CStaticMeshObject
{
public:
	CCharacter();
	virtual ~CCharacter();

	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj,
		LIGHT& Light, CAMERA& Camera) override;

	//弾を飛ばしたいか確認.
	bool IsShot() const { return m_Shot; }

	//Y軸方向へ伸ばしたレイを取得.
	RAY GetRayY() const { return *m_pRayY;  }
	//十字レイを取得.
	CROSSRAY GetCrossRay() const { return *m_pCrossRay;  }

protected:
	bool		m_Shot;			//弾を飛ばすフラグ.

	RAY*		m_pRayY;		//Y方向へ伸ばしたレイ.
	CROSSRAY*	m_pCrossRay;	//十字（前後左右に伸ばした）レイ
};

