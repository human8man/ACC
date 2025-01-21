#pragma once

#include "Mesh/Static/CStaticMeshObject.h"
#include "Collision/Ray/CRay.h"
#include "Object/Bullet/CBullet.h"
#include "Object/Gun/CGun.h"
#include "Collision/GJK/CGJK.h"
#include "Common/Time/CTime.h"

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
		int MaxHP;
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

	void DecreHP()		{ m_CharaInfo.HP--; }	 // HPを減らす.
	void DubleDecreHP()	{ m_CharaInfo.HP -= 2; } // HPを二倍減らす.


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

	// 銃関連.
	float m_GunRadius;		// 銃の半径.
	float m_GunRotRevision;	// 銃の向き補正値.
	float m_GunPosRevision;	// 銃の回転座標補正値(度).
	float m_Gravity;		// 実際に掛かる重力.
	float m_GravityValue;	// 重力（定数）.

	// 弾関連.
	float m_ReloadTime;			// リロードタイム.
	float m_ReloadTimeMax;		// リロード最大タイム.
	float m_BulletCoolTime;		// 銃の発射クールタイム.
	float m_BulletCoolTimeMax;	// 銃の発射最大クールタイム.
	float m_BulletSpeed;		// 弾速度.
	bool  m_CanShot;			// 発射可能か.

	// ジャンプ関連.
	float m_JumpPower;		// ジャンプ力.
	float m_JumpPowerMax;	// 最大ジャンプ力.
	bool m_CanJump;			// ジャンプが可能か.

	// ダッシュ関連.
	float m_DashCoolTime;	// ダッシュのクールタイム.
	float m_DashCoolTimeMax;// ダッシュの最大クールタイム.
	float m_DashTime;		// ダッシュの反映時間.
	float m_DashTimeMax;	// ダッシュの最大反映時間.
	float m_DashSpeed;		// ダッシュ倍率.
	bool m_CanDash;			// ダッシュが可能か.
	D3DXVECTOR3	DashVec;	// ダッシュのベクトル保存用.




	float m_EggAirRoomY;	// 気室の高さ判定用.

	CharaInfo m_CharaInfo;	// キャラの情報.
};