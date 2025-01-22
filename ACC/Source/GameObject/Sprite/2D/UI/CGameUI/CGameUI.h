#pragma once

#include "Sprite/2D/UI/CUIObject.h"

class CGameUI
	:public CUIObject
{
public:
	// 画像種類ごとに番号を振り当てる.
	enum GameSprite {
		Bullets,
		Crosshair,
		LowHP,
		Reload
	};

public:
	CGameUI();
	~CGameUI();

	void Create();
	void Update();
	void Draw();
	void Release();

	// Hit情報の設定.
	void SetHit(int hit);

	// 残弾数の設定.
	void SetAmmo( int val )	{ m_Ammo = val; }
	
	// HPの設定.
	void SetHP( int val, int max)	{ m_HP = val; m_HPMax = max;}

	// リロード時間の設定.
	void SetReloadTime( float time ){ m_ReloadTime = time; }

private:
	std::vector<std::string> m_SpriteDataList;	//スプライト情報をまとめる配列.
	std::vector<D3DXVECTOR3> m_SpritePosList;	//スプライト座標をまとめる配列.

	std::vector<CUIObject*> m_pUIs;			// UIクラス.
	std::vector<CSprite2D*> m_pSprite2Ds;	// Sprite2Dクラス.

	int		m_HP;				// HP.
	int		m_HPMax;			// 最大HP.
	int		m_Ammo;				// 残弾数.
	int		m_HitKind;			// Hitの種類.
	float	m_ReloadTime;		// リロード時間.
	float	m_ViewHitTime;		// ヒット表示時間.
	float	m_ViewHitTimeMax;	// ヒット最大表示時間.
};