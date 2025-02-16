#pragma once

#include "Sprite/2D/UI/CUIObject.h"
#include "Character/Player/CPlayer.h"

class CGameUI
	:public CUIObject
{
public:
	// 画像種類ごとに番号を振り当てる.
	enum GameSprite {
		Bullets,
		Crosshair,
		HP,
		LowHP,
		Reload,
		AutoAim,
		Homing
	};

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
	std::vector<std::string> m_SpriteDataList;	// スプライト情報をまとめる配列.
	std::vector<D3DXVECTOR3> m_SpritePosList;	// スプライト座標をまとめる配列.

	std::vector<CUIObject*> m_pUIs;			// UIクラス.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2Dクラス.

	int		m_HP;				// HP.
	int		m_HPMax;			// 最大HP.
	int		m_Ammo;				// 残弾数.
	int		m_HitKind;			// Hitの種類.
	float	m_ReloadTime;		// リロード時間.
	float	m_ViewHitTime;		// ヒット表示時間.
	float	m_ViewHitTimeMax;	// ヒット最大表示時間.

	bool	m_AutoAim;	// オートエイム.
	bool	m_Homing;	// ホーミング.
};