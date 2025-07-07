#pragma once

#include "Time/Time.h"
#include "Collision/GJK/GJK.h"
#include "Collision/Ray/Ray.h"
#include "Collision/Line/MeshLine.h"
#include "Mesh/Static/StaticMeshObject.h"
#include "Mesh/Static/Object/Gun/Gun.h"
#include "Mesh/Static/Object/Bullet/Bullet.h"


class Character
	: public StaticMeshObject
{
public:
	// キャラ情報構造体
	struct CharaInfo
	{
		int HP;
		int MaxHP;
		int Ammo;
		int MaxAmmo;
	};
public:
	Character();
	virtual ~Character();

	// 更新処理
	virtual void Update() override;
	// 描画処理
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj,LIGHT& Light) override;


	void JumpMath();		// ジャンプ関係の計算
	void CanJump()		{ m_CanJump = true;}	// ジャンプを可能にする
	// HPを胴体ダメージ分減らす
	void BodyDamage();
	// HPをクリティカルダメージ分減らす
	void CritDamage();

	// 最終移動時のベクトルを渡す
	D3DXVECTOR3 GetMoveVec();
	RAY GetRayY()			const { return *m_pRayY; }
	float GetReloadTime()	const { return m_ReloadTime; }
	float GetReloadTimeMax()const { return m_ReloadTimeMax; }
	float GetEggAirRoomY()	const { return m_EggAirRoomY; }
	CROSSRAY GetCrossRay()	const { return *m_pCrossRay; }
	CharaInfo GetCharaInfo()const { return m_CharaInfo; }
	bool GetDamage()		const{ return m_Damage; }
	bool GetCanJump()		const{ return m_CanJump; }
	bool GetLanding()		const{ return m_Landing; }
	bool GetRotGun()		const { return m_RotGun; }
	void SetLanding(bool flag)	{ m_Landing = flag; }

	std::vector<std::unique_ptr<Bullet>>& GetBullets()	{ return m_pBullets; }
protected:
	std::unique_ptr<RAY>		m_pRayY;		// Y方向へ伸ばしたレイ
	std::unique_ptr<CROSSRAY>	m_pCrossRay;	// 十字（前後左右に伸ばした）レイ
	
	std::unique_ptr<StaticMesh>			m_pMeshGun;		// 銃メッシュ
	std::unique_ptr<StaticMesh>			m_pMeshBullet;	// 弾メッシュ
	std::unique_ptr<MeshLine>				m_pMeshLine;	// メッシュ線クラス
	std::unique_ptr<Gun>					m_pGun;			// 銃クラス
	std::vector<std::unique_ptr<Bullet>>	m_pBullets;		// 弾クラス配列

	// ヒット時に使うダメージ変数
	int m_BodyDamage;	// 胴体に命中していた場合
	int m_CritDamage;	// 気室に命中(ヘッドショット)していた場合
	bool m_Damage;		// ダメージフラグ(次フレームで初期化)

	// 銃関連
	float m_GunRadius;		// 銃の半径
	float m_GunRotRevision;	// 銃の向き補正値
	float m_GunPosRevision;	// 銃の回転座標補正値(度)

	// 弾関連
	float m_ReloadTime;			// リロードタイム
	float m_ReloadTimeMax;		// リロード最大タイム
	float m_BulletCoolTime;		// 銃の発射クールタイム
	float m_BulletCoolTimeMax;	// 銃の発射最大クールタイム
	float m_BulletCoolTimeMin;	// 銃の発射最小クールタイム
	float m_BulletSpeed;		// 弾速度
	bool  m_CanShot;			// 発射可能か

	// ジャンプ関連
	float	m_Gravity;		// 重力
	float	m_JumpPower;	// ジャンプ力
	float	m_JumpPowerMax;	// 最大ジャンプ力
	bool	m_CanJump;		// ジャンプが可能か
	bool	m_Landing;		// 着地しているか

	// ダッシュ関連
	float	m_DashCoolTime;		// ダッシュのクールタイム
	float	m_DashCoolTimeMax;	// ダッシュの最大クールタイム
	float	m_DashTime;			// ダッシュの反映時間
	float	m_DashTimeMax;		// ダッシュの最大反映時間
	float	m_DashSpeed;		// ダッシュ倍率
	bool	m_CanDash;			// ダッシュが可能か
	D3DXVECTOR3	m_DashVec;		// ダッシュのベクトル保存用

	// 気室(ヘッドショット)の判定用
	float m_EggAirRoomY;

	// キャラ情報
	CharaInfo m_CharaInfo;
	// 合計の移動ベクトル量
	D3DXVECTOR3 m_SumVec;

	bool m_RotGun;
	float m_RotSpeed;
};