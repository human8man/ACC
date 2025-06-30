#pragma once

#include "Character/Character.h"
#include "Character/Player/Player.h"


class Enemy
	: public Character
{
public :
	// 移動の種類
	enum MoveKind {
		Straight,
		Left,
		Right,
		Back,
		Wait,

		max,
	};

public:
	Enemy();
	virtual ~Enemy();

	// 更新処理
	void Update(std::unique_ptr<Player>& chara);
	// 描画処理
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light );

	// 外的なベクトルの追加
	void AddVec(D3DXVECTOR3 vec) { m_vPosition += vec; }

private:
	// 行動をまとめる
	void Act(std::unique_ptr<Player>& chara);

private:
	// プレイヤー情報取得用
	Player*  m_pPlayer;
	// GJKクラス
	std::unique_ptr<GJK> m_pGJK;

	float m_MoveSpeed;	// 移動速度

	bool m_WallHack;	// プレイヤーがウォールハック使用中か
	bool m_Hit;			// 命中したか
	int	 m_HitKind;		// 命中の種類

	float m_SelectMoveTime;		// 次の行動決定までの時間
	float m_SelectMoveTimeMax;	// 次の行動決定までの最大時間

	int	m_MoveKind;			// 行動種類
	int	m_MoveToPlayer;		// プレイヤーへの移動確率(最大100%)
	int m_DashProbability;	// ダッシュを使う確率
	int m_JumpProbability;	// ジャンプを使う確率
	int m_ShootProbability;	// 射撃する確率
};