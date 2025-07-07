#include "GameUI.h"

#include "DirectX/DirectX11.h"
#include "DirectSound/SoundManager.h"
#include "Time/Time.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "Easing/Easing.h"

namespace {
	// UICSVのパス
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	std::string GameJson = "Data\\Texture\\UIData\\Game.json";
}


GameUI::GameUI()
	: m_HP				( 0 )
	, m_HPMax			( 0 )
	, m_Ammo			( 0 )
	, m_HitKind			( 0 )
	, m_ReloadRot		( 0.f )
	, m_ReloadTime		( 0.f )
	, m_ReloadTimeMax	( 0.f )
	, m_ViewHitTime		( 0.f )
	, m_ViewHitTimeMax	( Time::GetDeltaTime() * 60.f )
	, m_AutoAim			( false )
	, m_Homing			( false )
	, m_WallHack		( false )
	, m_TriggerHappy	( false )
	, m_Slow			( false )
	, m_Invincible		( false )
	, m_ThroughWall		( false )
	, m_RotGun			( false )
	
	, m_CrosshairEase		( 0.f )
	, m_CrosshairEaseMax	( Time::GetDeltaTime() * 150.f )
	, m_CrosshairRot		( 0.f )

	, m_HPRate				( 1.f )
	, m_DamageEase			( 0.f )
	, m_DamageEaseMax		( Time::GetDeltaTime() * 500.f )

	, m_PlayerDamage			( false )
	, m_ChangedRed				( false )
	, m_HitEffectColor			( 1.f )
	, m_HitEffectAlpha			( 0.f )
	, m_HitEffectTime			( 0.f )
	, m_HitEffectTimeMax		( Time::GetDeltaTime() * 30.f)
	, m_HitEffectChangeTime		( 0.f )
	, m_HitEffectChangeTimeMax	( Time::GetDeltaTime() * 10.f )
	, m_HitEffectDeleteTime		( 0.f )
	, m_HitEffectDeleteTimeMax	( Time::GetDeltaTime() * 50.f )
{
	// キャラクターCSVの情報保存用
	std::unordered_map<std::string, std::string> m_StateList;
	// キャラクターCSVの情報取得
	m_StateList = FileManager::CSVLoad(UICSVPath);

	// 空でない場合は、外部で調整するべき変数の値を入れていく
	if (!m_StateList.empty())
	{
		m_ViewHitTimeMax = StrToFloat(m_StateList["Game_HitViewTime"]) * Time::GetInstance()->GetDeltaTime();
	}
}
GameUI::~GameUI()
{
	Release();
}


//=================================================================================================
//		構築関数
//=================================================================================================
void GameUI::Create()
{
	LoadFromJson(GameJson, m_pUIs);
}


//=================================================================================================
//		更新
//=================================================================================================
void GameUI::Update(std::unique_ptr<Player>& chara)
{
	// カウントを回す
	if ( m_ViewHitTime >= 0.f ) { m_ViewHitTime -= Time::GetDeltaTime(); }
	if ( m_DamageEase <= m_DamageEaseMax ) { m_DamageEase += Time::GetDeltaTime(); }
	if ( m_CrosshairEase <= m_CrosshairEaseMax ) { m_CrosshairEase += Time::GetDeltaTime(); }
	
		
	// プレイヤーの攻撃が命中していた場合
	if (chara->GetHit()) {
		// 命中の種類を設定
		m_HitKind = chara->GetHitKind();
		m_ViewHitTime = m_ViewHitTimeMax;
		m_CrosshairEase = 0.f;
		m_CrosshairRot = 0.f;
	}

	// ゲーム画面UIに必要な情報の設定
	m_Homing		= chara->GetHoming();
	m_AutoAim		= chara->GetAutoAim();
	m_WallHack		= chara->GetWallHack();
	m_ReloadTime	= chara->GetReloadTime();
	m_TriggerHappy	= chara->GetTriggerHappy();
	m_Invincible	= chara->GetInvincible();
	m_ThroughWall	= chara->GetThroughWall();
	m_RotGun		= chara->GetRotGun();
	m_ReloadTimeMax = chara->GetReloadTimeMax();
	m_HP			= chara->GetCharaInfo().HP;
	m_Ammo			= chara->GetCharaInfo().Ammo;
	m_HPMax			= chara->GetCharaInfo().MaxHP;
	if (chara->GetDamage()) { InitHiteffect(); }
}


//=================================================================================================
//		描画
//=================================================================================================
void GameUI::Draw()
{
	for ( size_t i = 0; i < m_pUIs.size(); ++i ) {
		std::string spritename = m_pUIs[i]->GetSpriteData().Name;
		
		// 弾UIの描画設定
		if( spritename == "Bullets" ) {
			// リロードしていた場合は描画しない
			if (m_ReloadTime >= 0.f) { continue; }
			if (m_TriggerHappy) { m_pUIs[i]->SetPatternNo( 6, 0 );}
			else {m_pUIs[i]->SetPatternNo(m_Ammo, 0);}
		}

		// クロスヘアの描画設定
		if ( spritename == "Crosshair" ) {
			Crosshair(m_pUIs[i]);
		}

		// HPUIの描画設定
		if ( spritename == "HP" ) {
			m_HPRate = MyEasing::OutCirc( m_DamageEase, m_DamageEaseMax,m_HPRate,static_cast<float>(m_HP) / m_HPMax );
			D3DXVECTOR3 color = { 1.f, m_HPRate, m_HPRate };
			m_pUIs[i]->SetColor( color );
			m_pUIs[i]->SetScale( m_HPRate, 1.f, 1.f );
			// イージング中に終了する場合の対策
			if (m_HP <= 0) {
				m_pUIs[i]->SetScale( 0.f, 1.f, 1.f );
			}
		}

		// HitEffectの描画設定
		if ( spritename == "HitEffect" ) {
			if ( m_PlayerDamage ) { Hiteffect( m_pUIs[i] ); }
			m_pUIs[i]->SetAlpha( m_HitEffectAlpha );
		}

		// リロードUIの描画設定
		if ( spritename == "Reload" ) {
			if ( 0 < m_Ammo && m_ReloadTime < 0.f || m_TriggerHappy ) { m_ReloadRot = 0; continue; }
			if (m_ReloadTime >= 0.f) {
				m_ReloadRot = MyEasing::InOutCirc(m_ReloadTimeMax - m_ReloadTime, m_ReloadTimeMax, m_ReloadRot, 12.56f);
			}
			m_pUIs[i]->SetRot(0.f, 0.f, m_ReloadRot);
		}

		// リロードUI中央のRの描画設定
		if (spritename == "ReloadR") {
			if ( 0 < m_Ammo && m_ReloadTime < 0.f || m_TriggerHappy ) { continue; }
		}

		// リロードUIの描画設定
		if ( spritename == "Infinity" ) { if ( !m_TriggerHappy ) { continue; } }

		// チート系のUI描画設定
		if (spritename == "AutoAim")		{ m_pUIs[i]->SetPatternNo( m_AutoAim,		0 ); }
		if (spritename == "Homing")			{ m_pUIs[i]->SetPatternNo( m_Homing,		0 ); }
		if (spritename == "WallHack")		{ m_pUIs[i]->SetPatternNo( m_WallHack,		0 ); }
		if (spritename == "TriggerHappy")	{ m_pUIs[i]->SetPatternNo( m_TriggerHappy,	0 ); }
		if (spritename == "Slow")			{ m_pUIs[i]->SetPatternNo( m_Slow,			0 ); }
		if (spritename == "Invincible")		{ m_pUIs[i]->SetPatternNo( m_Invincible,	0 ); }
		if (spritename == "ThroughWall")	{ m_pUIs[i]->SetPatternNo( m_ThroughWall,	0 ); }
		if (spritename == "RotationGun")	{ m_pUIs[i]->SetPatternNo( m_RotGun,		0 ); }

		m_pUIs[i]->Draw();
	}
}


//-----------------------------------------------------------------------------
//		クロスヘア処理
//-----------------------------------------------------------------------------
void GameUI::Crosshair(UIObject* object)
{
	if (m_ViewHitTime > 0.f) {
		object->SetPatternNo(m_HitKind, 0);
		if (m_HitKind == 2) {
			m_CrosshairRot = MyEasing::OutCirc(m_CrosshairEase, m_CrosshairEaseMax, m_CrosshairRot, 3.14f);
		}
		else {
			m_CrosshairRot = MyEasing::OutCirc(m_CrosshairEase, m_CrosshairEaseMax, m_CrosshairRot, 1.57f);
		}
		object->SetRot(0.f, 0.f, m_CrosshairRot);
	}
	else {
		object->SetPatternNo(0, 0);
		object->SetRot(0.f, 0.f, 0.f);
	}

}


//-----------------------------------------------------------------------------
//		被ダメエフェクトを初期化
//-----------------------------------------------------------------------------
void GameUI::InitHiteffect()
{
	m_DamageEase		= 0.f;

	m_PlayerDamage		= true;
	m_ChangedRed		= false;
	m_HitEffectColor	= 1.f;
	m_HitEffectAlpha	= 0.f;
	m_HitEffectTime			= 0.f;
	m_HitEffectTimeMax		= Time::GetDeltaTime() * 10.f;
	m_HitEffectChangeTime		= 0.f;
	m_HitEffectChangeTimeMax	= Time::GetDeltaTime() * 10.f;
	m_HitEffectDeleteTime		= 0.f;
	m_HitEffectDeleteTimeMax	= Time::GetDeltaTime() * 15.f ;
}


//-----------------------------------------------------------------------------
//		被ダメエフェクト処理
//-----------------------------------------------------------------------------
void GameUI::Hiteffect(UIObject* object)
{
	D3DXVECTOR3 color = { 1.f,m_HitEffectColor,m_HitEffectColor };
	float goalalpha = 0.2f;
	if (m_HP < m_HPMax / 2) { goalalpha = 0.4f; }
	// 変色前
	if (!m_ChangedRed)
	{
		m_HitEffectTime += Time::GetDeltaTime();
		// 透過無しにする
		m_HitEffectAlpha = MyEasing::OutExpo(m_HitEffectTime, m_HitEffectTimeMax,m_HitEffectAlpha, goalalpha);
		// 透過でない場合
		if (goalalpha <= m_HitEffectAlpha) {
			// 赤以外の色要素を消す
			m_HitEffectChangeTime += Time::GetDeltaTime();
			m_HitEffectColor = MyEasing::OutExpo(m_HitEffectChangeTime, m_HitEffectChangeTimeMax, m_HitEffectColor, 0.f);
			// 完了時にフラグを立てる
			if (m_HitEffectColor <= 0.f) { m_ChangedRed = true; }
		}
	}
	else {
		// エフェクト事態を透過
		m_HitEffectDeleteTime += Time::GetDeltaTime();
		m_HitEffectAlpha = MyEasing::OutExpo(m_HitEffectDeleteTime, m_HitEffectDeleteTimeMax, m_HitEffectAlpha, 0.f);
		if (m_HitEffectAlpha <= 0.f) { m_PlayerDamage = 0.f; }
	}
	object->SetColor(color);
}


//=================================================================================================
//		解放処理
//=================================================================================================
void GameUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}