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
		"HitEffect",
		"Reload",
		"Infinity",
		"AutoAim",
		"Homing",
		"WallHack",
		"TriggerHappy"
	};
}


//=================================================================================================
//		ゲームUIクラス.
//=================================================================================================
CGameUI::CGameUI()
	: m_HP						( 0 )
	, m_HPMax					( 0 )
	, m_Ammo					( 0 )
	, m_HitKind					( 0 )
	, m_ReloadTime				( 0.f )
	, m_ViewHitTime				( 0.f )
	, m_ViewHitTimeMax			( CTime::GetInstance()->GetDeltaTime() * 60.f )
	, m_AutoAim					( false )
	, m_Homing					( false )
	, m_WallHack				( false )
	, m_TriggerHappy			( false )
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
	if (m_ViewHitTime >= 0.f) { m_ViewHitTime -= CTime::GetInstance()->GetDeltaTime(); }

	// プレイヤーの攻撃が命中していた場合.
	if (chara->GetHit()) {
		// 命中の種類を設定.
		m_HitKind = chara->GetHitKind();
		m_ViewHitTime = m_ViewHitTimeMax;
	}

	// ゲーム画面UIに必要な情報の設定.
	m_Homing		= chara->GetHoming();
	m_WallHack		= chara->GetWallHack();
	m_TriggerHappy	= chara->GetTriggerHappy();
	m_AutoAim		= chara->GetAutoAim();
	m_ReloadTime	= chara->GetReloadTime();
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
			if (m_TriggerHappy) { m_pUIs[i]->SetPatternNo(6, 0);}
			else {m_pUIs[i]->SetPatternNo(m_Ammo, 0);}
		}

		// クロスヘアの描画設定.
		if ( spritename == "Crosshair" ) {
			if (m_ViewHitTime > 0.f) {
				m_pUIs[i]->SetPatternNo(m_HitKind, 0);
			}
			else {
				m_pUIs[i]->SetPatternNo(0, 0);
			}
		}

		// HPUIの描画設定.
		if ( spritename == "HP" ) {
			float hprate = static_cast<float>(m_HP) / m_HPMax;
			D3DXVECTOR3 color = { 1.f,hprate,hprate };
			m_pUIs[i]->SetColor(color);
			m_pUIs[i]->SetScale(hprate,1.f ,1.f);
		}

		// HitEffectの描画設定.
		if ( spritename == "HitEffect" ) {
			if (m_PlayerDamage) { Hiteffect(m_pUIs[i]); }
			m_pUIs[i]->SetAlpha(m_HitEffectAlpha);
		}

		// リロードUIの描画設定.
		if ( spritename == "Reload" ) {
			// リロードしていない場合は描画しない.
			if (m_ReloadTime <= 0.f || m_TriggerHappy) { continue; }
		}
		// リロードUIの描画設定.
		if ( spritename == "Infinity" ) {
			// リロードしていない場合は描画しない.
			if (!m_TriggerHappy) { continue; }
		}

		// チート系のUI描画設定.
		if (spritename == "AutoAim")		{ m_pUIs[i]->SetPatternNo(m_AutoAim, 0);		}
		if (spritename == "Homing")			{ m_pUIs[i]->SetPatternNo(m_Homing, 0);			}
		if (spritename == "WallHack")		{ m_pUIs[i]->SetPatternNo(m_WallHack, 0);		}
		if (spritename == "TriggerHappy")	{ m_pUIs[i]->SetPatternNo(m_TriggerHappy, 0);	}

		m_pUIs[i]->Draw();
	}
}


//=================================================================================================
//		解放処理.
//=================================================================================================
void CGameUI::Release()
{
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pUIs[i]); }
	for (size_t i = 0; i < m_SpriteDataList.size(); ++i) { SAFE_DELETE(m_pSprite2Ds[i]); }
}


//=================================================================================================
//		被ダメエフェクトを初期化.
//=================================================================================================
void CGameUI::InitHiteffect()
{
	m_PlayerDamage		= true;
	m_ChangedRed		= false;
	m_HitEffectColor	= 1.f;
	m_HitEffectAlpha	= 0.f;
	m_HitEffectTime			= 0.f;
	m_HitEffectTimeMax		= CTime::GetInstance()->GetDeltaTime() * 10.f;
	m_HitEffectChangeTime		= 0.f;
	m_HitEffectChangeTimeMax	= CTime::GetInstance()->GetDeltaTime() * 10.f;
	m_HitEffectDeleteTime		= 0.f;
	m_HitEffectDeleteTimeMax	= CTime::GetInstance()->GetDeltaTime() * 15.f ;
}


//=================================================================================================
//		被ダメエフェクト処理.
//=================================================================================================
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