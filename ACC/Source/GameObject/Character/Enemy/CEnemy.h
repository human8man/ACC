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


	// 移動量の取得.
	const D3DXVECTOR3& GetMoveVec() { return m_SumVec; }
	// 外的なベクトルの追加.
	void AddVec(D3DXVECTOR3 vec) { m_vPosition += vec; }
	// ジャンプ力の減少.
	void JumpPowerDec() { if (m_JumpPower > 0) { m_JumpPower -= m_Gravity; } }

private:
	// 行動をまとめる.
	void Act(std::unique_ptr<CPlayer>& chara);

private:
	// プレイヤー情報取得用.
	CPlayer*  m_pPlayer;
	// GJKクラス.
	std::unique_ptr<CGJK> m_pGJK;

	float m_TurnSpeed;			// 回転速度.
	float m_MoveSpeed;			// 移動速度.
	float m_CamRevision;		// カメラ座標の補正値.

	bool m_Hit;					// 命中したか.
	int	 m_HitKind;				// 命中の種類.

	float m_SelectMoveTime;		// 次の行動決定までの時間.
	float m_SelectMoveTimeMax;	// 次の行動決定までの最大時間.

	int			m_MoveKind;		// 行動種類.
	D3DXVECTOR3 m_SumVec;		// 合計のベクトル量.
};