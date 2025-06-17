#pragma once
#include "Character/CCharacter.h"
#include "DirectInput/CDirectInput.h"

#pragma warning(push)
#pragma warning(disable: 4834)

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

	// 更新処理.
	void Update(std::unique_ptr<CEnemy>& chara);
	// 描画処理.
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light );
	
	const int& GetHitKind()			const { return m_HitKind;		}	// 命中の種類を渡す.
	const bool& GetHit()			const { return m_Hit;			}	// 命中フラグを渡す.
	const bool& GetHoming()			const { return m_Homing;		}	// ホーミングフラグを渡す.
	const bool& GetAutoAim()		const { return m_AutoAim;		}	// オートエイムフラグを渡す.
	const bool& GetWallHack()		const { return m_WallHack;	}	// ウォールハックフラグを渡す.
	const bool& GetTriggerHappy()	const { return m_TriggerHappy;}	// 連射モードフラグを渡す.

	// 移動量の追加.
	void AddVec(D3DXVECTOR3 vec) { m_vPosition += vec; }
	// 当たり判定処理(エネミー、床、柱).
	void Collision(std::unique_ptr<CEnemy>& egg, Collider floor, Collider cylinder);
private:
	// キー入力処理.
	void KeyInput(std::unique_ptr<CEnemy>& chara);
	// 移動処理.
	void MoveProcess(std::unique_ptr<CEnemy>& chara);
	// ダッシュ処理.
	void DashProcess();
	// 発射処理.
	void ShotProcess();

	// デバッグ時に使用する関数.
	void PlayerImGui();

private:
	// GJKクラス.
	std::unique_ptr<CGJK> m_pGJK;

	float m_MoveSpeed;	 // 移動速度.
	float m_CamRevision; // カメラ座標の補正値.

	bool m_Hit;			// 命中したか.
	int	 m_HitKind;		// 命中の種類.

	bool m_AutoAim;		// オートエイム.
	bool m_Homing;		// ホーミング.
	bool m_WallHack;	// ウォールハック.
	bool m_TriggerHappy;// 連射モード.
};