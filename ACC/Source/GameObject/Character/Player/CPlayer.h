#pragma once
#include "Character/CCharacter.h"

class CEnemy;

// Hit情報.
enum HitKind {
	Hit = 1,
	Crit
};

//============================================================================
//		プレイヤークラス.
//============================================================================
class CPlayer
	: public CCharacter
{
public:
	CPlayer();
	virtual ~CPlayer() override;

	void Update(std::unique_ptr<CEnemy>& chara);
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;


	// 命中フラグを渡す.
	const bool& GetHit() { return m_Hit; }
	const int& GetHitKind() { return m_HitKind; }
	const bool& GetAutoAim() { return m_AutoAim; }
	const bool& GetHoming() { return m_Homing; }
	// 移動量の取得.
	const D3DXVECTOR3& GetMoveVec() { return m_SumVec; }

	// 移動量の追加.
	void AddVec(D3DXVECTOR3 vec) { m_vPosition += vec; }
	void JumpPowerDec() { if (m_JumpPower > 0) { m_JumpPower -= m_Gravity; } }

	// 当たり判定処理(エネミー、床、柱).
	void Collision(std::unique_ptr<CEnemy>& egg, MeshCollider floor, MeshCollider cylinder);

private:
	// キー入力処理.
	void KeyInput(std::unique_ptr<CEnemy>& chara);

private:
	// GJKクラス.
	std::unique_ptr<CGJK> m_pGJK;

	float m_TurnSpeed;	 // 回転速度.
	float m_MoveSpeed;	 // 移動速度.
	float m_CamRevision; // カメラ座標の補正値.

	bool m_Hit;		// 命中したか.
	int	 m_HitKind;	// 命中の種類.

	bool m_AutoAim;	// オートエイム.
	bool m_Homing;	// ホーミング.

	D3DXVECTOR3 m_SumVec;	// 合計のベクトル量.
};