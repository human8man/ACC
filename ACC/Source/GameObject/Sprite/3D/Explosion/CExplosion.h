#pragma once
#include "../GameObject/Sprite/CSpriteObject.h"

/**************************************************
*	爆発クラス.
**/
class CExplosion
	: public CSpriteObject	//スプライトオブジェクトクラスを継承.
{
public:
	CExplosion();
	virtual ~CExplosion() override;

	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj ) override;

	void ResetAnimation(){
		m_PatternNo.x = 0;
		m_PatternNo.y = 0;
		m_AnimCount = 0;
	}

protected:
	POINTS	m_PatternNo;	//パターン番号(マス目).
	int		m_AnimCount;	//アニメーションカウント.
};
