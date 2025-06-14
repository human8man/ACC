#pragma once

#include "Character/CCharacter.h"
#include "Character/Player/CPlayer.h"


//=============================================================================
//		エネミークラス.
//=============================================================================
class CEnemy
	: public CCharacter
{
public :
	// 移動の種類.
	enum MoveKind {
		Straight,
		Left,
		Right,
		Back,
		Wait,

		max,
	};

public:
	CEnemy();
	virtual ~CEnemy();

	// 更新処理.
	void Update(std::unique_ptr<CPlayer>& chara);
	// 描画処理.
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light );
	
	// 当たり判定処理(プレイヤー、床、柱).
	void Collision(std::unique_ptr<CPlayer>& egg, Collider floor, Collider cylinder);

	// 外的なベクトルの追加.
	void AddVec(D3DXVECTOR3 vec) { m_vPosition += vec; }

private:
	// 行動をまとめる.
	void Act(std::unique_ptr<CPlayer>& chara);

private:
	// プレイヤー情報取得用.
	CPlayer*  m_pPlayer;
	// GJKクラス.
	std::unique_ptr<CGJK> m_pGJK;

	float m_MoveSpeed;	// 移動速度.

	bool m_WallHack;	// プレイヤーがウォールハック使用中か.
	bool m_Hit;			// 命中したか.
	int	 m_HitKind;		// 命中の種類.

	float m_SelectMoveTime;		// 次の行動決定までの時間.
	float m_SelectMoveTimeMax;	// 次の行動決定までの最大時間.

	int	m_MoveKind;			// 行動種類.
	int	m_MoveToPlayer;		// プレイヤーへの移動確率(最大100%).
	int m_DashProbability;	// ダッシュを使う確率.
	int m_JumpProbability;	// ジャンプを使う確率.
	int m_ShootProbability;	// 射撃する確率.
};