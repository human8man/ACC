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
	// キャラ状態列挙体.
	enum CharaState
	{
		// ここに状態とか書いていく.
	};
	// チート状態列挙.
	enum CheatState
	{
		// ここに使うチートとか書いていく.
	};

	// キャラ情報構造体.
	struct CharaInfo
	{
		int HP;
		int Ammo;
		int MaxAmmo;
		CharaState Status;
		CheatState UsingCheat1;
		CheatState UsingCheat2;
		CheatState UsingCheat3;
		CheatState UsingCheat4;
	};
public:
	CCharacter();
	virtual ~CCharacter();

	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj,LIGHT& Light) override;

	// ジャンプ関連.
	void CanJump() { m_CanJump = true; } // ジャンプを可能にする.

	// 重力関連.
	void ResetGravity()	{ m_Gravity		 = m_GravityValue; } // 重力をリセットする.
	void AddGravity()	{ m_Gravity		+= m_GravityValue; } // 重力を加算していく.
	void UseGravity()	{ m_vPosition.y -= m_Gravity;	   } // 加算した重力をキャラのY値に減算.

	// Y軸方向へ伸ばしたレイを取得.
	RAY GetRayY() const { return *m_pRayY; }
	// 十字レイを取得.
	CROSSRAY GetCrossRay() const { return *m_pCrossRay; }
	// キャラの情報を取得.
	CharaInfo GetCharaInfo() const { return m_CharaInfo; }
protected:
	RAY*		m_pRayY;		// Y方向へ伸ばしたレイ.
	CROSSRAY*	m_pCrossRay;	// 十字（前後左右に伸ばした）レイ
	
	CStaticMesh*			m_pMeshBullet;	// 弾メッシュ.
	CStaticMesh*			m_pMeshGun;		// 銃メッシュ.
	std::vector<CBullet*>	m_pBullets;		// 弾配列.
	CGun*					m_pGun;			// 銃.

	float m_GunRadius;		// 銃の半径.
	float m_GunRotRevision;	// 銃の向き補正値.
	float m_GunPosRevision;	// 銃の回転座標補正値(度).
	float m_Gravity;		// 実際に掛かる重力.
	float m_GravityValue;	// 重力（定数）.

	float m_JumpPower;		// ジャンプ力.
	float m_JumpPowerMax;	// 最大ジャンプ力.
	bool m_CanJump;			// ジャンプが可能か.

	CharaInfo m_CharaInfo;	// キャラの情報.
};