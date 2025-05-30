#pragma once

#include "Sprite/2D/UI/CUIObject.h"
#include "Character/Player/CPlayer.h"

class CGameUI
	:public CUIObject
{
public:
	CGameUI();
	~CGameUI();

	// 作成処理.
	void Create();
	// 更新処理.
	void Update(std::unique_ptr<CPlayer>& chara);
	// 描画処理.
	void Draw(); 
	// 解放処理.
	void Release(); 

private:
	int		m_HP;				// HP.
	int		m_HPMax;			// 最大HP.
	int		m_Ammo;				// 残弾数.
	int		m_HitKind;			// Hitの種類.
	float	m_ReloadTime;		// リロード時間.
	float	m_ViewHitTime;		// ヒット表示時間.
	float	m_ViewHitTimeMax;	// ヒット最大表示時間.

	bool	m_AutoAim;		// オートエイム.
	bool	m_Homing;		// ホーミング.
	bool	m_WallHack;		// ウォールハック.
	bool	m_TriggerHappy;	// 連射モード.
};