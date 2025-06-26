#pragma once

#include "Sprite/2D/UI/UIObject.h"
#include "Character/Player/CPlayer.h"


class GameUI
	:public UIObject
{
public:
	GameUI();
	~GameUI();

	// 作成処理
	void Create();
	// 更新処理
	void Update(std::unique_ptr<CPlayer>& chara);
	// 描画処理
	void Draw(); 
	// 解放処理
	void Release(); 

	// スローUI設定関数
	void SetSlowUI(bool flag) { m_Slow = flag; }
private:
	// 被ダメエフェクト処理
	void InitHiteffect();
	void Hiteffect(UIObject* object);

	// クロスヘア処理
	void Crosshair(UIObject* object);

private:
	int		m_HP;				// HP
	int		m_HPMax;			// 最大HP
	int		m_Ammo;				// 残弾数
	int		m_HitKind;			// Hitの種類

	float	m_ReloadRot;		// リロード回転
	float	m_ReloadTime;		// リロード時間
	float	m_ReloadTimeMax;	// リロード最大時間

	float	m_ViewHitTime;		// ヒット表示時間
	float	m_ViewHitTimeMax;	// ヒット最大表示時間

	bool	m_AutoAim;		// オートエイム
	bool	m_Homing;		// ホーミング
	bool	m_WallHack;		// ウォールハック
	bool	m_TriggerHappy;	// 連射モード
	bool	m_Slow;			// スローモード

	// クロスヘアのイージングに使用
	float	m_CrosshairEase;
	float	m_CrosshairEaseMax;
	float	m_CrosshairRot;

	// ダメージ時のHP減少に使用
	float	m_HPRate;
	float	m_DamageEase;
	float	m_DamageEaseMax;

	bool	m_PlayerDamage;				// 被ダメフラグ
	bool	m_ChangedRed;				// 色相変化後
	float	m_HitEffectColor;			// 色相変化用
	float	m_HitEffectAlpha;			// 被ダメ透過値
	float	m_HitEffectTime;			// 被ダメ経過時間
	float	m_HitEffectTimeMax;			// 被ダメ最大時間
	float	m_HitEffectChangeTime;		// 被ダメ変色経過時間
	float	m_HitEffectChangeTimeMax;	// 被ダメ変色最大時間
	float	m_HitEffectDeleteTime;		// 被ダメ削除経過時間
	float	m_HitEffectDeleteTimeMax;	// 被ダメ削除最大時間
};