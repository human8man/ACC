#pragma once
#include "Character/Character.h"
#include "DirectInput/DirectInput.h"

#pragma warning(push)
#pragma warning(disable: 4834)

class Enemy;

// Hit情報
enum HitKind {
	Hit = 1,
	Crit
};


class Player
	: public Character
{
public:
	Player();
	virtual ~Player() override;

	// 更新処理
	void Update(std::unique_ptr<Enemy>& chara);
	// 描画処理
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light);

	void SetLanding(bool flag)		{ m_Landing = flag; }
	const bool& GetLanding()		const { return m_Landing; }


	void SetHit(bool flag)			{ m_Hit = flag; }
	void SetHitKind(HitKind kind)	{ m_HitKind = kind; }
	const int& GetHitKind()			const { return m_HitKind; }
	const bool& GetHit()			const { return m_Hit; }
	const bool& GetHoming()			const { return m_Homing; }
	const bool& GetAutoAim()		const { return m_AutoAim; }
	const bool& GetWallHack()		const { return m_WallHack; }
	const bool& GetTriggerHappy()	const { return m_TriggerHappy; }
	const bool& GetInvincible()		const { return m_Invincible; }

	// 移動量の追加
	void AddVec(D3DXVECTOR3 vec) { m_vPosition += vec; }
private:
	// キー入力処理
	void KeyInput(std::unique_ptr<Enemy>& chara);
	// 移動処理
	void MoveProcess(std::unique_ptr<Enemy>&chara);
	// ダッシュ処理
	void DashProcess();
	// 発射処理
	void ShotProcess();

	// デバッグ時に使用する関数
	void PlayerImGui();

private:
	// GJKクラス
	std::unique_ptr<GJK> m_pGJK;

	float m_MoveSpeed;	 // 移動速度
	float m_CamRevision; // カメラ座標の補正値

	bool m_Hit;			// 命中したか
	int	 m_HitKind;		// 命中の種類

	bool m_AutoAim;		// オートエイム
	bool m_Homing;		// ホーミング
	bool m_WallHack;	// ウォールハック
	bool m_TriggerHappy;// 連射モード
	bool m_Invincible;	// 無敵
};