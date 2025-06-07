#include "CGameUI.h"

#include "DirectX/CDirectX11.h"
#include "DirectSound/CSoundManager.h"
#include "Time/CTime.h"
#include "FileManager/FileManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "Easing/Easing.h"

namespace {
	// UICSVのパス.
	constexpr char UICSVPath[] = "Data\\CSV\\UIStatus.csv";
	// UIリスト.
	std::vector<std::string> ImageList = {
		"Bullets",
		"Crosshair",
		"HP",
		"HPFrame",
		"HitEffect",
		"Reload",
		"ReloadR",
		"Infinity",
		"AutoAim",
		"Homing",
		"WallHack",
		"TriggerHappy",
		"Slow"
	};
}


//=================================================================================================
//		ゲームUIクラス.
//=================================================================================================
CGameUI::CGameUI()
	: m_HP				( 0 )
	, m_HPMax			( 0 )
	, m_Ammo			( 0 )
	, m_HitKind			( 0 )
	, m_ReloadRot		( 0.f )
	, m_ReloadTime		( 0.f )
	, m_ReloadTimeMax	( 0.f )
	, m_ViewHitTime		( 0.f )
	, m_ViewHitTimeMax	( CTime::GetDeltaTime() * 60.f )
	, m_AutoAim			( false )
	, m_Homing			( false )
	, m_WallHack		( false )
	, m_TriggerHappy	( false )
	, m_Slow			( false )
	
	, m_CrosshairEase		( 0.f )
	, m_CrosshairEaseMax	( CTime::GetDeltaTime() * 150.f )

	, m_HPRate				( 1.f )
	, m_DamageEase			( 0.f )
	, m_DamageEaseMax		( CTime::GetDeltaTime() * 500.f )

	, m_PlayerDamage			( false )
	, m_ChangedRed				( false )
	, m_HitEffectColor			( 1.f )
	, m_HitEffectAlpha			( 0.f )
	, m_HitEffectTime			( 0.f )
	, m_HitEffectTimeMax		( CTime::GetDeltaTime() * 30.f)
	, m_HitEffectChangeTime		( 0.f )
	, m_HitEffectChangeTimeMax	( CTime::GetDeltaTime() * 10.f )
	, m_HitEffectDeleteTime		( 0.f )
	, m_HitEffectDeleteTimeMax	( CTime::GetDeltaTime() * 50.f )
{
	// キャラクターCSVの情報保存用.
	std::unordered_map<std::string, std::string> m_StateList;
	// キャラクターCSVの情報取得.
	m_StateList = FileManager::CSVLoad(UICSVPath);

	// 空でない場合は、外部で調整するべき変数の値を入れていく.
	if (!m_StateList.empty())
	{
		m_ViewHitTimeMax = StrToFloat(m_StateList["Game_HitViewTime"]) * CTime::GetInstance()->GetDeltaTime();
	}
}
CGameUI::~CGameUI()
{
	Release();
}


//=================================================================================================
//		構築関数.
//=================================================================================================
void CGameUI::Create()
{
	LoadSpriteList(ImageList, m_pUIs, m_pSprite2Ds);
}


//=================================================================================================
//		更新.
//=================================================================================================
void CGameUI::Update(std::unique_ptr<CPlayer>& chara)
{
	// カウントを回す.
	if ( m_ViewHitTime >= 0.f ) { m_ViewHitTime -= CTime::GetDeltaTime(); }
	if ( m_DamageEase <= m_DamageEaseMax ) { m_DamageEase += CTime::GetDeltaTime(); }
	if ( m_CrosshairEase <= m_CrosshairEaseMax ) { m_CrosshairEase += CTime::GetDeltaTime(); }
	
		
	// プレイヤーの攻撃が命中していた場合.
	if (chara->GetHit()) {
		// 命中の種類を設定.
		m_HitKind = chara->GetHitKind();
		m_ViewHitTime = m_ViewHitTimeMax;
		m_CrosshairEase = 0.f;
		m_CrosshairRot = 0.f;
	}

	// ゲーム画面UIに必要な情報の設定.
	m_Homing		= chara->GetHoming();
	m_AutoAim		= chara->GetAutoAim();
	m_WallHack		= chara->GetWallHack();
	m_ReloadTime	= chara->GetReloadTime();
	m_TriggerHappy	= chara->GetTriggerHappy();
	m_ReloadTimeMax = chara->GetReloadTimeMax();
	m_HP			= chara->GetCharaInfo().HP;
	m_Ammo			= chara->GetCharaInfo().Ammo;
	m_HPMax			= chara->GetCharaInfo().MaxHP;
	if (chara->GetDamage()) { InitHiteffect(); }
}


//=================================================================================================
//		描画.
//=================================================================================================
void CGameUI::Draw()
{
	for ( size_t i = 0; i < m_pUIs.size(); ++i ) {
		std::string spritename = m_pUIs[i]->GetSpriteData().Name;
		
		// 弾UIの描画設定.
		if( spritename == "Bullets" ) {
			// リロードしていた場合は描画しない.
			if (m_ReloadTime >= 0.f) { continue; }
			if (m_TriggerHappy) { m_pUIs[i]->SetPatternNo( 6, 0 );}
			else {m_pUIs[i]->SetPatternNo(m_Ammo, 0);}
		}

		// クロスヘアの描画設定.
		if ( spritename == "Crosshair" ) {
			Crosshair(m_pUIs[i]);
		}

		// HPUIの描画設定.
		if ( spritename == "HP" ) {
			m_HPRate = MyEasing::OutCirc( m_DamageEase, m_DamageEaseMax,m_HPRate,static_cast<float>(m_HP) / m_HPMax );
			D3DXVECTOR3 color = { 1.f, m_HPRate, m_HPRate };
			m_pUIs[i]->SetColor( color );
			m_pUIs[i]->SetScale( m_HPRate, 1.f, 1.f );
			// イージング中に終了する場合の対策.
			if (m_HP <= 0) {
				m_pUIs[i]->SetScale( 0.f, 1.f, 1.f );
			}
		}

		// HitEffectの描画設定.
		if ( spritename == "HitEffect" ) {
			if ( m_PlayerDamage ) { Hiteffect( m_pUIs[i] ); }
			m_pUIs[i]->SetAlpha( m_HitEffectAlpha );
		}

		// リロードUIの描画設定.
		if ( spritename == "Reload" ) {
			if ( 0 < m_Ammo && m_ReloadTime < 0.f || m_TriggerHappy ) { m_ReloadRot = 0; continue; }
			if (m_ReloadTime >= 0.f) {
				m_ReloadRot = MyEasing::InOutCirc(m_ReloadTimeMax - m_ReloadTime, m_ReloadTimeMax, m_ReloadRot, 12.56f);
			}
			m_pUIs[i]->SetRot(0.f, 0.f, m_ReloadRot);
		}

		// リロードUI中央のRの描画設定.
		if (spritename == "ReloadR") {
			if ( 0 < m_Ammo && m_ReloadTime < 0.f || m_TriggerHappy ) { continue; }
		}

		// リロードUIの描画設定.
		if ( spritename == "Infinity" ) { if ( !m_TriggerHappy ) { continue; } }

		// チート系のUI描画設定.
		if (spritename == "AutoAim")		{ m_pUIs[i]->SetPatternNo( m_AutoAim,		0 ); }
		if (spritename == "Homing")			{ m_pUIs[i]->SetPatternNo( m_Homing,		0 ); }
		if (spritename == "WallHack")		{ m_pUIs[i]->SetPatternNo( m_WallHack,		0 ); }
		if (spritename == "TriggerHappy")	{ m_pUIs[i]->SetPatternNo( m_TriggerHappy,	0 ); }
		if (spritename == "Slow")			{ m_pUIs[i]->SetPatternNo( m_Slow,			0 ); }

		m_pUIs[i]->Draw();
	}
}


//-----------------------------------------------------------------------------
//		クロスヘア処理.
//-----------------------------------------------------------------------------
void CGameUI::Crosshair(CUIObject* object)
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
//		被ダメエフェクトを初期化.
//-----------------------------------------------------------------------------
void CGameUI::InitHiteffect()
{
	m_DamageEase		= 0.f;

	m_PlayerDamage		= true;
	m_ChangedRed		= false;
	m_HitEffectColor	= 1.f;
	m_HitEffectAlpha	= 0.f;
	m_HitEffectTime			= 0.f;
	m_HitEffectTimeMax		= CTime::GetDeltaTime() * 10.f;
	m_HitEffectChangeTime		= 0.f;
	m_HitEffectChangeTimeMax	= CTime::GetDeltaTime() * 10.f;
	m_HitEffectDeleteTime		= 0.f;
	m_HitEffectDeleteTimeMax	= CTime::GetDeltaTime() * 15.f ;
}


//-----------------------------------------------------------------------------
//		被ダメエフェクト処理.
//-----------------------------------------------------------------------------
void CGameUI::Hiteffect(CUIObject* object)
{
	D3DXVECTOR3 color = { 1.f,m_HitEffectColor,m_HitEffectColor };
	float goalalpha = 0.2f;
	if (m_HP < m_HPMax / 2) { goalalpha = 0.4f; }
	// 変色前
	if (!m_ChangedRed)
	{
		m_HitEffectTime += CTime::GetDeltaTime();
		// 透過無しにする.
		m_HitEffectAlpha = MyEasing::OutExpo(m_HitEffectTime, m_HitEffectTimeMax,m_HitEffectAlpha, goalalpha);
		// 透過でない場合.
		if (goalalpha <= m_HitEffectAlpha) {
			// 赤以外の色要素を消す.
			m_HitEffectChangeTime += CTime::GetDeltaTime();
			m_HitEffectColor = MyEasing::OutExpo(m_HitEffectChangeTime, m_HitEffectChangeTimeMax, m_HitEffectColor, 0.f);
			// 完了時にフラグを立てる.
			if (m_HitEffectColor <= 0.f) { m_ChangedRed = true; }
		}
	}
	else {
		// エフェクト事態を透過.
		m_HitEffectDeleteTime += CTime::GetDeltaTime();
		m_HitEffectAlpha = MyEasing::OutExpo(m_HitEffectDeleteTime, m_HitEffectDeleteTimeMax, m_HitEffectAlpha, 0.f);
		if (m_HitEffectAlpha <= 0.f) { m_PlayerDamage = 0.f; }
	}
	object->SetColor(color);
}


//=================================================================================================
//		解放処理.
//=================================================================================================
void CGameUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}